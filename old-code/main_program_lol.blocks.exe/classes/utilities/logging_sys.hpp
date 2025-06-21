/****************************************************************
	Header: logging_sys.hpp

	Purpose: keep track of log messages

****************************************************************/



#ifndef LOGGING_SYSTEM_HEADER_HPP
#define LOGGING_SYSTEM_HEADER_HPP

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error handling
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"


namespace utilities {
	
	// manages memory for base logger
	// deletes objects in the destructor
	class logger {
	public:
		logger() = default;
		~logger();
	protected:

		// object for holding a message and time
		class base_logger {
		public:
			base_logger() = default;

			// cleans up new memory
			~base_logger();

			// sets the pointer m_log_message_p to message and time stamps it
			errors::codes log_a_message(const string& message);
		private:
			// object for housing a string time stamp
			class time_str {
			public:
				// reserve memory here
				time_str();

				// call this to set the string to the current time
				errors::codes set_current_time();

				// the actual string container
				string m_time_str;

				// the reserved size of the string
				static const size_t reserved_length_heap_mem = 32;
			};
		public:
			// object for housing a log message
			class log_message {
			public:
				// reserve the log message size
				log_message();

				// set the log message to message
				errors::codes set_log_message(const string& message);

				// max length of the m_message
				static const size_t reserved_length_mem_heap = 256;

				// the actual string message
				string m_message;
			};
		private:

			// log message pointer for base_logger
			log_message* m_log_message_p = new log_message;

			// time stamp pointer for base_logger
			time_str* m_time_str_p = new time_str;
		public:

			// get a pointer to base_logger's log message pointer
			log_message* get_message_p() { return m_log_message_p; }

			// get a pointer to base_logger's time_str pointer
			time_str* get_time_p() { return m_time_str_p; }
		};


	public:

		// handles the actual log messages using a vector of
		// base_logger pointers
		class logs {
		public:
			// reserves the vector memory size 
			// fills the vector with base_logger pointers
			logs();

			// cleans up base_logger pointers
			~logs();

			// returns the current position of the current log
			// when a log is set it increments m_index_pos by 1
			// it will cycle through the m_bl_vec_p over and over
			// rewriting each base_logger's m_message string
			// the actual log message
			size_t get_index_pos() { return m_index_pos; }

			// this function is the function to be called
			// when logging a message to the logger
			// the other log message functions in base_logger are meant for internal use only
			errors::codes log_message(const string& message);

			// returns the log at m_index_pos
			string get_most_recent_log();

			// returns a log from m_bl_vec_p at an index position in the vector
			string get_log_by_index(size_t index = 0);

			// returns current size of the m_bl_vec_p vector
			// it will match the reserved capacity
			size_t get_vec_log_size() { return m_bl_vec_p.size(); }

			// returns a base_logger pointer from m_bl_vec_p at an index position
			base_logger* get_base_logger_p_by_index(size_t index = 0);
		private:
			// vector of base_logger pointers, each pointer holds 
			// a log message (includes time stamp)
			std::vector<base_logger*> m_bl_vec_p = {};

			// also the total number of logs stored
			// we dont allocate any more than whats reserved
			static const size_t m_bl_vec_reserved_capacity = 500;

			// current position in m_bl_vec_p vector for outputting
			// that log
			size_t m_index_pos = 0;

			// prevent concurrent access when logging a message
			std::mutex m_message_mtx;
		};
	private:
		// logs object pointer wrapped for safe multithreading dereferencing
		std::atomic<logs*>* m_stored_logs_p = new std::atomic<logs*>(new logs);
	public:
		// use this pointer for actual logging
		// and call log_message()
		std::atomic<logs*>* get_logs_p() { return m_stored_logs_p; }
	};
}

// LOGGING_SYSTEM_HEADER_HPP
#endif