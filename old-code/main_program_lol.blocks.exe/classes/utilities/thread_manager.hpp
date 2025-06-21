/****************************************************************
	Header: thread_manager.hpp

	Purpose: wrapper class around std::thread/std::jthread to keep
			 track of threads.

****************************************************************/



#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

namespace utilities {
	
	class thread_master {
	public:
		thread_master() = default;
		~thread_master();

		// Wrapper to launch a thread
		template<typename FunctionType, typename... Args>
		std::jthread* launch_thread(FunctionType&& func, Args&&... args) {
			// Create a callable object using std::bind
			auto bound_func = std::bind(std::forward<FunctionType>(func), std::forward<Args>(args)...);

			// Wrap in std::function to match create_new_thread signature
			auto callable = std::function<void()>(bound_func);

			std::jthread* thread_obj = new std::jthread(callable);

			{
				std::unique_lock<std::mutex> local_lock(thread_mp_mtx);
				m_thread_p_id_mp.emplace(thread_obj->get_id(), thread_obj);
			}
			
			return thread_obj;
		}

		// Wrapper to launch the master thread
		template<typename FunctionType, typename... Args>
		std::jthread* launch_master_thread(FunctionType&& func, Args&&... args) {
			// Create a callable object using std::bind
			auto bound_func = std::bind(std::forward<FunctionType>(func), std::forward<Args>(args)...);

			// Wrap in std::function to match create_new_thread signature
			auto callable = std::function<void()>(bound_func);

			m_master_thread = new std::jthread(callable);
			m_master_thread_id = m_master_thread->get_id();
			return m_master_thread;
		}


		void join_all() {
			for (auto pair : m_thread_p_id_mp) {
				if (pair.second->joinable()) {
					pair.second->join();
				}
			}
		}

		void detach_all() {
			for (auto pair : m_thread_p_id_mp) {
				if (pair.second->joinable()) {
					pair.second->detach();
				}
			}
		}

		void join_one(std::thread::id ID) {
			auto found_thread = m_thread_p_id_mp.find(ID);
			if (found_thread != m_thread_p_id_mp.end()) {
				if (found_thread->second->joinable()) {
					found_thread->second->join();
				}
			}
		}

		void detach_one(std::thread::id ID) {
			auto found_thread = m_thread_p_id_mp.find(ID);
			if (found_thread != m_thread_p_id_mp.end()) {
				if (found_thread->second->joinable()) {
					found_thread->second->detach();
				}
			}
		}

		void join_master() {
			if (m_master_thread->joinable()) {
				m_master_thread->join();
			}
		}
	protected:
		std::mutex thread_mp_mtx;
		std::unordered_map < std::thread::id, std::jthread*> m_thread_p_id_mp = {};

		std::jthread* m_master_thread = nullptr;
		std::thread::id m_master_thread_id;
	};
}