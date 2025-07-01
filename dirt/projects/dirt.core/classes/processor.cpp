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
{
    m_watch_entries_v = get_specific_entries(entry_v,args::watch);
    m_copy_entries_v = get_specific_entries(entry_v, args::copy);
}

core::process::~process()
{
    clean_up();
}

core::codes core::process::process_entry()
{
    {
        std::cout << "Processing copy entries...\n";
        codes code = process_copy_entries();
        if (code != codes::success) {
            return code;
        }
    }
    
    
    {
        std::cout << "Processing watch entries...\n";
        codes code = process_watch_entries();
        if (code != codes::success) {
            return code;
        }
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
                entry.src_s = std::filesystem::status(entry.src_p);
            }
            catch (const std::filesystem::filesystem_error& e) {
                output_em(std_filesystem_exception_caught_pkg);
                output_fse(e);
            }
            catch (...) {
                output_em(unknown_exception_caught_pkg);
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
            exit_process_entry();

            // queue_sys_t thread will auto join on exit

            return codes::read_dir_changes_fail;
        }
    }

    return codes::success;
}

core::codes core::process::process_watch_entries()
{
    m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (m_hIOCP == nullptr) {
        return codes::invalid_io_handle;
    }

    for (auto& entry : m_watch_entries_v) {
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

core::codes core::process::process_copy_entries()
{
    std::vector<codes> errors;
    
    for (const auto& entry : m_copy_entries_v) {
        std::uintmax_t size = total_size(entry.src_p);
        auto found = std::find(entry.args_v.begin(), entry.args_v.end(),args::recursive);
        if (found != entry.args_v.end()) {
            
            auto start = std::chrono::high_resolution_clock::now();
            codes code = copy_directory_recursive(entry.dst_p, entry.src_p);
            auto end = std::chrono::high_resolution_clock::now();

            if (code != codes::success) {
                errors.push_back(code);
                auto pkg = match_code(code);
                output_entry(entry);
                output_em(pkg);
            }

            std::chrono::duration<double> elapsed = end - start;
            double seconds = elapsed.count();

            double speed = size / seconds; // bytes per second

            output_entry(entry);
            std::cout << "Copied " << size << " bytes in " << seconds << " seconds.\n";
            std::cout << "Speed: " << (speed / (1024 * 1024)) << " MB/s\n";
        }
        else {
            auto start = std::chrono::high_resolution_clock::now();
            codes code = copy_directory_only(entry.dst_p, entry.src_p);
            auto end = std::chrono::high_resolution_clock::now();

            if (code != codes::success) {
                errors.push_back(code);
                auto pkg = match_code(code);
                output_entry(entry);
                output_em(pkg);
            }

            std::chrono::duration<double> elapsed = end - start;
            double seconds = elapsed.count();

            double speed = size / seconds; // bytes per second

            output_entry(entry);
            std::cout << "Copied " << size << " bytes in " << seconds << " seconds.\n";
            std::cout << "Speed: " << (speed / (1024 * 1024)) << " MB/s\n";
        }

    }

    for (auto error : errors) {
        // return first error
        return error;
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
    // background queue system thread
    std::jthread bqs_t(&core::queue_system::delayed_process_entry, this);
    
    
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

        std::vector<std::queue<file_entry>> fe_qv = split_queue(m_entry_buffer,MAX_THREADS);
        std::vector<std::jthread> pq_tv;

        for (auto q : fe_qv) {
            pq_tv.push_back(std::jthread(&core::queue_system::process_queue,this,q));
        }
        
        m_launch_b.store(false);
    }

    // exit background queue system
    exit_dpe();
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
        bool copied = false;
        try {
            if (std::filesystem::exists(entry.src_p) == false) {
                // the file has been deleted during the wait time
                break;
            }

            copied = std::filesystem::copy_file(entry.src_p, entry.dst_p,
                std::filesystem::copy_options::update_existing);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
        }

        if (copied == false) {
            std::cout << "File not copied: " << entry.src_p << '\n';
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
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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
            filesystem_ec(e,entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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
            output_em(unknown_exception_caught_pkg);
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

void core::queue_system::not_found(file_entry& entry)
{
    std::filesystem::file_status d_fs;
    try {
        d_fs = std::filesystem::status(entry.dst_p);
    }
    catch (const std::filesystem::filesystem_error& e) {
        output_em(std_filesystem_exception_caught_pkg);
        output_fse(e);
    }
    catch (...) {
        output_em(unknown_exception_caught_pkg);
    }
    
    switch (d_fs.type())
    {
    case std::filesystem::file_type::none:
        // do
        break;
    case std::filesystem::file_type::not_found:
        // means its been deleted already
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
}

void core::queue_system::background_task(const file_entry& entry)
{
    switch (entry.completed_action) {
    case directory_completed_action::recursive_copy:
    {
        try {
            auto set = entry.p_di_set;
            for (const auto& i : std::filesystem::recursive_directory_iterator(entry.dst_p)) {
                directory_info di;
                if (i.is_directory() == true) {
                    di.number_of_files = file_numbers(i.path());
                    di.p = i.path();
                    di.action = entry.completed_action;

                    {
                        std::unique_lock<std::mutex> local_lock(m_set_mtx);
                        set->emplace(di);
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
        }
        break;
    }

    case directory_completed_action::delete_all:
    {
        try {
            auto set = entry.p_di_set;
            for (const auto& i : std::filesystem::recursive_directory_iterator(entry.dst_p)) {
                if (i.is_directory() == true) {
                    directory_info di;
                    di.p = i.path();
                    
                    {
                        std::unique_lock<std::mutex> local_lock(m_set_mtx);
                        set->erase(di);
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
        }
        break;
    }

    case directory_completed_action::previous_name:
    {

        break;
    }

    case directory_completed_action::new_name:
    {

        break;
    }

    default:
        // do
        break;
    }
}

void core::queue_system::filesystem_ec(const std::filesystem::filesystem_error& e, const file_entry& entry)
{
    switch (e.code().value()) {
    case static_cast<int>(std::errc::no_such_file_or_directory):
        // do
        break;
    case static_cast<int>(std::errc::permission_denied):
        // do
        break;
    case static_cast<int>(std::errc::not_a_directory):
        // do
        break;
    case static_cast<int>(std::errc::is_a_directory):
        // do
        break;
    case static_cast<int>(std::errc::file_exists):
        // do
        break;
    case static_cast<int>(std::errc::directory_not_empty):
        // do
        break;
    case static_cast<int>(std::errc::device_or_resource_busy):
        add_delayed_entry(entry);
        break;
    case static_cast<int>(std::errc::read_only_file_system):
        // do
        break;
    case static_cast<int>(std::errc::no_space_on_device):
        // do
        break;
    default:
        // do
        break;
    }
}

void core::queue_system::switch_entry_type(file_entry& entry)
{
    switch (entry.src_s.type())
    {
    case std::filesystem::file_type::none:
        // do
        break;
    case std::filesystem::file_type::not_found:
        not_found(entry);
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
}

void core::queue_system::exit_process_entry()
{
    // signal queue system to finish
    m_launch_b.store(true);
    m_launch_cv.notify_all();

    // break while loop
    m_runner.store(false);
}

void core::queue_system::process_queue(std::queue<file_entry> buffer_q)
{
    // background task thread vector
    std::vector<std::thread> bgtv;
    
    while (buffer_q.empty() == false) {
        file_entry entry = buffer_q.front();
        output_entry_data(entry);

        switch_entry_type(entry);

        if (entry.completed_action != directory_completed_action::uninit) {
            bgtv.push_back(std::thread(&core::queue_system::background_task, this, entry));
        }

        buffer_q.pop();
    }

    for (auto& t : bgtv) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void core::background_queue_system::regular_file(file_entry& entry)
{
    switch (entry.action) {
    case file_action::copy:
    {
        bool copied = false;
        try {
            if (std::filesystem::exists(entry.src_p) == false) {
                // the file has been deleted during the wait time
                break;
            }

            copied = std::filesystem::copy_file(entry.src_p, entry.dst_p,
                std::filesystem::copy_options::update_existing);
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
        }

        if (copied == false) {
            std::cout << "File not copied: " << entry.src_p << '\n';
        }
        else {
            m_delayed_q.pop();
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
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
        }

        if (removed == false) {
            std::cout << "Failed to delete: " << entry.dst_p << '\n';
        }
        else {
            m_delayed_q.pop();
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

            m_delayed_q.pop();
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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

void core::background_queue_system::directory(file_entry& entry)
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

            m_delayed_q.pop();
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
        }

        if (removed == 0) {
            std::cout << "Failed to delete: " << entry.dst_p << '\n';
        }
        else {
            std::cout << "Removed files from: " << entry.dst_p << '\n'
                << "Total files removed: " << removed << '\n';

            entry.completed_action = directory_completed_action::delete_all;

            m_delayed_q.pop();
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

            m_delayed_q.pop();
        }
        catch (const std::filesystem::filesystem_error& e) {
            output_em(std_filesystem_exception_caught_pkg);
            output_fse(e);
            filesystem_ec(e, entry);
        }
        catch (...) {
            output_em(unknown_exception_caught_pkg);
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

void core::background_queue_system::not_found(file_entry& entry)
{
    std::filesystem::file_status d_fs;
    try {
        d_fs = std::filesystem::status(entry.dst_p);
    }
    catch (const std::filesystem::filesystem_error& e) {
        output_em(std_filesystem_exception_caught_pkg);
        output_fse(e);
    }
    catch (...) {
        output_em(unknown_exception_caught_pkg);
    }
    
    
    switch (d_fs.type())
    {
    case std::filesystem::file_type::none:
        // do
        break;
    case std::filesystem::file_type::not_found:
        // deleted already
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
}

void core::background_queue_system::switch_entry_type(file_entry& entry)
{
    switch (entry.src_s.type())
    {
    case std::filesystem::file_type::none:
        // do
        break;
    case std::filesystem::file_type::not_found:
        not_found(entry);
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
}

void core::background_queue_system::delayed_process_entry()
{
    while (m_run_dpe.load() == true) {
        
        // trigger here
        {
            std::unique_lock<std::mutex> local_lock(m_launch_dpe_mtx);
            m_launch_dpe_cv.wait(local_lock, [this]
                {
                    return m_launch_dpe_b.load();
                });
        }
        
        
        while (m_delayed_q.empty() == false and m_run_dpe.load() == true) {
            file_entry entry = m_delayed_q.front();

            output_entry_data(entry);
            switch_entry_type(entry);

            // timer here, seconds to wait time
            std::this_thread::sleep_for(std::chrono::seconds(BUFFER_TIME));
        }

        m_launch_dpe_b.store(false);
    }
}

void core::background_queue_system::add_delayed_entry(const file_entry& entry)
{
    std::unique_lock<std::mutex> local_lock(m_dq_mtx);
    m_delayed_q.emplace(entry);
}

void core::background_queue_system::filesystem_ec(const std::filesystem::filesystem_error& e, const file_entry& entry)
{
    switch (e.code().value()) {
    case static_cast<int>(std::errc::no_such_file_or_directory):
        // do
        break;
    case static_cast<int>(std::errc::permission_denied):
        // do
        break;
    case static_cast<int>(std::errc::not_a_directory):
        // do
        break;
    case static_cast<int>(std::errc::is_a_directory):
        // do
        break;
    case static_cast<int>(std::errc::file_exists):
        // do
        break;
    case static_cast<int>(std::errc::directory_not_empty):
        // do
        break;
    case static_cast<int>(std::errc::device_or_resource_busy):
        // do
        break;
    case static_cast<int>(std::errc::read_only_file_system):
        // do
        break;
    case static_cast<int>(std::errc::no_space_on_device):
        // do
        break;
    default:
        // do
        break;
    }
}

void core::background_queue_system::exit_dpe()
{
    // exit the while loop
    m_run_dpe.store(false);

    // trigger the thread to move
    m_launch_dpe_b.store(true);
    m_launch_dpe_cv.notify_all();
}
