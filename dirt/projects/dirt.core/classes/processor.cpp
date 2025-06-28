#include "processor.hpp"
/**********************************************************/
//
// File: processor.cpp
//
// Purpose: processor.hpp definitions
//
// Project: dirt.core
//
/**********************************************************/

#include CORE_NAMES_INCLUDE
#include CORE_PROCESSOR_INCLUDE_PATH

core::process::process(const std::vector<arg_entry>& entry_v)
    :m_entry_v(entry_v)
{

}

core::process::~process()
{
    clean_up();
}

core::codes core::process::process_entry()
{
    m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (m_hIOCP == nullptr) {
        return codes::invalid_io_handle;
    }
    
    for (auto& entry : m_entry_v) {
        HANDLE hDir = CreateFileW(
            entry.src_p.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL
        );

        if (hDir == nullptr) {
            return codes::handle_nullptr;
        }

        auto* ctx = new DirWatchContext;
        ctx->hDir = hDir;
        ctx->src = entry.src_p;
        ctx->dst = entry.dst_p;
        ctx->di_set = new std::unordered_set(get_all_directories(entry.src_p));

        m_hCompletionPort = CreateIoCompletionPort(hDir, m_hIOCP, (ULONG_PTR)ctx, 0);

        if (!ReadDirectoryChangesW(
            hDir,
            ctx->buffer,
            sizeof(ctx->buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
            NULL,
            &ctx->overlapped,
            NULL
        )) {
            CloseHandle(hDir);
            delete ctx;
            clean_up();
            return codes::read_dir_changes_fail;
        }

        m_file_handle_v.push_back(hDir);
        m_context_v.push_back(ctx);
        m_di_set_p_v.push_back(ctx->di_set);
    }
    
    return codes::success;
}

core::codes core::process::watch()
{
    std::jthread queue_sys_t(&queue_system::process_entry, this);
    
    
    while (
        GetQueuedCompletionStatus(m_hCompletionPort,&m_bytesTransferred,&m_completionKey,&m_pOverlapped,INFINITE)) {

        DirWatchContext* ctx = reinterpret_cast<DirWatchContext*>(m_completionKey);
        FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)ctx->buffer;

        do {
            std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
            
            // create an entry
            file_entry entry;
            entry.src_p = ctx->src / fileName;
            entry.dst_p = ctx->dst / fileName;
            entry.action = convert_action(pNotify->Action);
            entry.p_di_set = ctx->di_set;

            try {
                entry.s = std::filesystem::status(entry.src_p);
            }
            catch (const std::filesystem::filesystem_error& e) {
                output_em(std_filesystem_exception_caught_pkg);
                output_fse(e);
            }
            catch (...) {
                output_em(unknown_exception_caught_pkg);
                std::cout << "unknown exception caught...\n";
            }




            add_entry(entry);

            if (pNotify->NextEntryOffset == 0)
                break;
            pNotify = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify + pNotify->NextEntryOffset);
        } while (true);


        // signal queue system
        m_launch_b.store(true);
        m_launch_cv.notify_all();

        // Re-issue the read
        ZeroMemory(&ctx->overlapped, sizeof(OVERLAPPED));
        if (!ReadDirectoryChangesW(
            ctx->hDir,
            ctx->buffer,
            sizeof(ctx->buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
            NULL,
            &ctx->overlapped,
            NULL
        )) {
            // signal queue system to finish
            m_launch_b.store(true);
            m_launch_cv.notify_all();

            // break while loop
            m_runner.store(false);

            // queue_sys_t thread will auto join on exit

            return codes::read_dir_changes_fail;
        }
    }

    return codes::success;
}

core::file_action core::process::convert_action(DWORD action)
{
    switch (action) {
    case FILE_ACTION_ADDED:
        return file_action::copy;
    case FILE_ACTION_REMOVED:
        return file_action::delete_dst;
    case FILE_ACTION_MODIFIED:
        return file_action::modified;
    case FILE_ACTION_RENAMED_OLD_NAME:
        return file_action::previous_name;
    case FILE_ACTION_RENAMED_NEW_NAME:
        return file_action::new_name;
    default:
        return file_action::unknown;
    }
}

core::directory_completed_action core::process::convert_directory_action(DWORD action)
{
    switch (action) {
    case FILE_ACTION_ADDED:
        return directory_completed_action::recursive_copy;
    case FILE_ACTION_REMOVED:
        return directory_completed_action::delete_all;
    case FILE_ACTION_RENAMED_OLD_NAME:
        return directory_completed_action::previous_name;
    case FILE_ACTION_RENAMED_NEW_NAME:
        return directory_completed_action::new_name;
    default:
        return directory_completed_action::unknown;
    }
}

void core::process::clean_up()
{
    for (auto handle : m_file_handle_v) {
        CloseHandle(handle);
    }

    for (auto c : m_context_v) {
        if (c != nullptr) {
            delete c;
            c = nullptr;
        }
    }

    for (auto p : m_di_set_p_v) {
        if (p != nullptr) {
            delete p;
            p = nullptr;
        }
    }

    CloseHandle(m_hCompletionPort);
    CloseHandle(m_hIOCP);
}

void core::queue_system::process_entry()
{
    while (m_runner.load() == true) {
        
        // trigger here
        {
            std::unique_lock<std::mutex> local_lock(m_launch_mtx);
            m_launch_cv.wait(local_lock, [this]
                {
                    return m_launch_b.load();
                });

            // timer here, seconds to wait time
            std::this_thread::sleep_for(std::chrono::seconds(BUFFER_TIME));
        }




        {
            std::unique_lock<std::mutex> local_lock(m_queue_mtx);
            m_entry_q.swap(m_entry_buffer);
        }

        
        while (m_entry_buffer.empty() == false) {
            file_entry entry = m_entry_buffer.front();
            output_entry_data(entry);

            switch (entry.s.type())
            {
            case std::filesystem::file_type::none:
                // do
                break;
            case std::filesystem::file_type::not_found:
                // do
                break;
            case std::filesystem::file_type::regular:
                regular_file(entry);
                break;
            case std::filesystem::file_type::directory:
                directory(entry);
                break;
            case std::filesystem::file_type::symlink:
                // do
                break;
            case std::filesystem::file_type::block:
                // do
                break;
            case std::filesystem::file_type::character:
                // do
                break;
            case std::filesystem::file_type::fifo:
                // do
                break;
            case std::filesystem::file_type::socket:
                // do
                break;
            case std::filesystem::file_type::unknown:
                // do
                break;
            default:
                // do
                break;
            }

            if (entry.completed_action != directory_completed_action::uninit) {
                m_bgtv.push_back(std::thread(&background_task, entry));
            }
            
            m_entry_buffer.pop();

        }

        for (auto& t : m_bgtv) {
            if (t.joinable()) {
                t.join();
            }
        }

        m_launch_b.store(false);
        m_bgtv.clear();
    }
}

void core::queue_system::add_entry(const file_entry& entry)
{
    std::unique_lock<std::mutex> local_lock(m_queue_mtx);
    m_entry_q.emplace(entry);
}

void core::queue_system::regular_file(file_entry& entry)
{
    switch (entry.action) {
    case file_action::copy:
    {

        try {
            if (std::filesystem::exists(entry.src_p) == false) {
                // the file has been deleted during the wait time
                break;
            }

            std::filesystem::copy_file(entry.src_p, entry.dst_p,
                std::filesystem::copy_options::update_existing);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
            std::cout << "unknown exception caught...\n";
        }

        break;
    }

    case file_action::delete_dst:
    {

        bool removed = false;
        try {
            if (std::filesystem::exists(entry.dst_p) == false) {
                // the file has been deleted manually
                break;
            }

            removed = std::filesystem::remove(entry.dst_p);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
            std::cout << "unknown exception caught...\n";
        }

        if (removed == false) {
            std::cout << "Failed to delete: " << entry.dst_p << '\n';
        }

        break;
    }

    case file_action::modified:
    {
        try {
            if (std::filesystem::exists(entry.src_p) == false) {
                // the file has been deleted during the wait time
                break;
            }

            std::filesystem::copy_file(entry.src_p, entry.dst_p,
                std::filesystem::copy_options::update_existing);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
            std::cout << "unknown exception caught...\n";
        }

        break;
    }

    case file_action::new_name:
    {
        // do
        break;
    }

    case file_action::previous_name:
    {
        // do
        break;
    }

    case file_action::uninit:
    {
        // do
        break;
    }

    default:
        break;
    } // switch end
}

void core::queue_system::directory(file_entry& entry)
{
    switch (entry.action) {
    case file_action::copy:
    {

        try {
            if (std::filesystem::exists(entry.src_p) == false) {
                // the directory has been deleted during the wait time
                break;
            }

            std::filesystem::copy(entry.src_p, entry.dst_p,
                std::filesystem::copy_options::update_existing | 
                std::filesystem::copy_options::recursive);

            entry.completed_action = directory_completed_action::recursive_copy;
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            std::cout << "unknown exception caught...\n";
        }

        break;
    }

    case file_action::delete_dst:
    {
        
        std::uintmax_t removed = 0;
        try {
            if (std::filesystem::exists(entry.dst_p) == false) {
                // the directory has been deleted manually
                break;
            }

            removed = std::filesystem::remove_all(entry.dst_p);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            std::cout << "unknown exception caught...\n";
        }

        if (removed == 0) {
            std::cout << "Failed to delete: " << entry.dst_p << '\n';
        }
        else {
            std::cout << "Removed files from: " << entry.dst_p << '\n' 
                << "Total files removed: " << removed << '\n';

            entry.completed_action = directory_completed_action::delete_all;
        }

        break;
    }

    case file_action::modified:
    {
        try {
            if (std::filesystem::exists(entry.src_p) == false) {
                // the directory has been deleted during the wait time
                break;
            }

            std::filesystem::copy(entry.src_p, entry.dst_p,
                std::filesystem::copy_options::update_existing);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            std::cout << "unknown exception caught...\n";
        }

        break;
    }

    case file_action::new_name:
    {
        // do
        break;
    }

    case file_action::previous_name:
    {
        // do
        break;
    }

    case file_action::uninit:
    {
        // do
        break;
    }

    default:
        break;
    } // switch end

}


