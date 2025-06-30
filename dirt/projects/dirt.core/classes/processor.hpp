/**********************************************************/
//
// File: processor.hpp
//
// Purpose: process file entries
//
// Project: dirt.core
//
/**********************************************************/

#pragma once

#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH
#include CORE_CODES_INCLUDE_PATH
#include CORE_ARGS_INCLUDE_PATH
#include CORE_API_INCLUDE_PATH

#include <Windows.h>

// 4 MB Buffer
#define BUFFER_SIZE 1024 * 1024 * 4
#define BUFFER_TIME 10

namespace core {
	class background_queue_system {
	public:
		background_queue_system() = default;

		void delayed_process_entry();

		void add_delayed_entry(const file_entry& entry);
	protected:
		virtual void regular_file(file_entry& entry);
		virtual void directory(file_entry& entry);
		virtual void not_found(file_entry& entry);

		virtual void switch_entry_type(file_entry& entry);

		std::queue<file_entry> m_delayed_q;

		virtual void filesystem_ec(const std::filesystem::filesystem_error& e, const file_entry& entry);

		// mutex for m_delayed_q
		std::mutex m_dq_mtx;
		
		// delayed process entry loop runner
		std::atomic<bool> m_run_dpe = true;

		// mutex for launching delayed process entry
		std::mutex m_launch_dpe_mtx;

		// condition variable for launching delayed process entry
		std::condition_variable m_launch_dpe_cv;

		// boolean for launching delayed process entry
		std::atomic<bool> m_launch_dpe_b;

		// exit delayed process entry function
		void exit_dpe();
	};


	class queue_system : public background_queue_system {
	public:
		queue_system() = default;

		void process_entry();

		void add_entry(const file_entry& entry);
	protected:
		void regular_file(file_entry& entry) override;
		void directory(file_entry& entry) override;
		void not_found(file_entry& entry) override;

		std::queue<file_entry> m_entry_buffer;
		std::queue<file_entry> m_entry_q;
		
		std::mutex m_queue_mtx;

		// background task thread vector
		std::vector<std::thread> m_bgtv;

		// process entry loop runner
		std::atomic<bool> m_runner = true;

		// mutex for launching process entry
		std::mutex m_launch_mtx;

		// condition variable for launching process entry
		std::condition_variable m_launch_cv;

		// boolean for launching process entry
		std::atomic<bool> m_launch_b;

		void background_task(const file_entry& entry);
		
		// mutex for std::unordered_set<directory_info>* di_set in each entry
		std::mutex m_set_mtx;

		void filesystem_ec(const std::filesystem::filesystem_error& e, const file_entry& entry) override;

		void switch_entry_type(file_entry& entry) override;

		void exit_process_entry();
	};


	struct DirWatchContext {
		OVERLAPPED overlapped = {};
		BYTE buffer[BUFFER_SIZE] = {};
		HANDLE hDir = {nullptr};
		std::filesystem::path src;
		std::filesystem::path dst;
		std::unordered_set<directory_info>* di_set = nullptr;
	};


	class process : public queue_system{
	public:
		process(const std::vector<arg_entry>& entry_v);
		~process();

		codes process_entry();

		codes watch();
	protected:
		codes process_watch_entries();
		codes process_copy_entries();

		std::vector<arg_entry> m_watch_entries_v;
		std::vector<arg_entry> m_copy_entries_v;


		std::vector<DirWatchContext*> m_context_v;

		std::vector<HANDLE> m_file_handle_v;

		DWORD m_bytesTransferred = 0;
		ULONG_PTR m_completionKey = {};
		LPOVERLAPPED m_pOverlapped = {nullptr};
		HANDLE m_hCompletionPort = { nullptr };
		HANDLE m_hIOCP = {nullptr};

		file_action convert_action(DWORD action);
		directory_completed_action convert_directory_action(DWORD action);

		std::vector<std::unordered_set<directory_info>*> m_di_set_p_v;

		void clean_up();
	};


	
}