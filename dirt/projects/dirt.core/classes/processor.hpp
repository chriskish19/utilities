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
	class queue_system {
	public:
		queue_system() = default;

		void process_entry();

		void add_entry(const file_entry& entry);
	protected:
		void regular_file(file_entry& entry);
		void directory(file_entry& entry);

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
		std::vector<arg_entry> m_entry_v;

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