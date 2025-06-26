/**********************************************************/
//
// File: args.hpp
//
// Purpose: list of arguments used on the cmdline
//
// Project: dirt.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH


namespace core {
	enum class args {
		copy,
		uninit = 1,
		watch,
		dirt_list_path,
		mirror,
		new_files_only,
		no_deletes,
		unknown,
		src,
		dst,

	};


	class arg_pkg {
	public:
		arg_pkg(args arg, std::string s_arg) 
		:m_arg(arg),m_s_arg(s_arg){}

		const args m_arg;
		const std::string m_s_arg;
	};

	inline const arg_pkg copy_pkg(args::copy, "-copy");
	inline const arg_pkg uninit_pkg(args::uninit, "-uninit");
	inline const arg_pkg watch_pkg(args::watch, "-watch");
	inline const arg_pkg dirt_list_path_pkg(args::dirt_list_path, "-dirt_list_path");
	inline const arg_pkg mirror_pkg(args::mirror, "-mirror");
	inline const arg_pkg new_files_only_pkg(args::new_files_only, "-new_files_only");
	inline const arg_pkg no_deletes_pkg(args::no_deletes, "-no_deletes");
	inline const arg_pkg unknown_pkg(args::unknown, "-unknown");
	inline const arg_pkg src_pkg(args::src, "src");
	inline const arg_pkg dst_pkg(args::dst, "dst");


	inline const std::unordered_map<std::string, args> gbl_args_mp = {
		{"-copy", args::copy},
		{"-uninit" , args::uninit},
		{"-watch", args::watch},
		{"-dirt_list_path", args::dirt_list_path},
		{"-mirror",args::mirror},
		{"-new_files_only",args::new_files_only},
		{"-no_deletes", args::no_deletes},
		{"-unknown", args::unknown},
		{"src" , args::src},
		{"dst" , args::dst}
	};

	class base_group {
	public:
		// src directory
		args arg1 = args::src;

		// destination directory
		args arg2 = args::dst;

		virtual bool match_group(const std::vector<args>& arg_v) {
			std::size_t src_count = 0;
			std::size_t dst_count = 0;
			for (auto arg : arg_v) {
				if (arg == arg1) {
					src_count++;
				}
				if (arg == arg2) {
					dst_count++;
				}
			}

			return src_count == 1 and dst_count == 1;
		}
	};

	class group_2 : public base_group {
	public:
		// initial copy of src to dst, and copy any future changes
		args arg1 = args::copy;  

		// mirrors src to dst, any changes in src are reflected in dst
		args arg2 = args::mirror;

		// watches the directory for changes
		args arg3 = args::watch;

		virtual bool match_group(const std::vector<args>& arg_v) override {
			bool base_match = base_group::match_group(arg_v);

			if (base_match == false) {
				return base_match;
			}


			std::size_t copy_count = 0;
			std::size_t mirror_count = 0;
			std::size_t watch_count = 0;
			for (auto arg : arg_v) {
				if (arg == arg1) {
					copy_count++;
				}
				if (arg == arg2) {
					mirror_count++;
				}
				if (arg == arg3) {
					watch_count++;
				}
			}

			return copy_count == 1 and mirror_count == 1 and watch_count == 1 and base_match == true;
		}
	};

	class group_3 : public base_group {
	public:
		// files added to src are added to dst but files removed from src are not removed from dst
		args arg1 = args::no_deletes;

		// watches the directory for changes
		args arg2 = args::watch;

		virtual bool match_group(const std::vector<args>& arg_v) override {
			bool base_match = base_group::match_group(arg_v);

			if (base_match == false) {
				return base_match;
			}

			std::size_t no_deletes_count = 0;
			std::size_t watch_count = 0;
			for (auto arg : arg_v) {
				if (arg == arg1) {
					no_deletes_count++;
				}
				if (arg == arg2) {
					watch_count++;
				}
			}

			return no_deletes_count == 1 and watch_count == 1 and base_match == true;
		}
	};

	class group_4 : public base_group {
	public:
		// files added to src are added to dst but files removed from src are not removed from dst
		args arg1 = args::no_deletes;

		// watches the directory for changes
		args arg2 = args::watch;

		// initial copy of src to dst, and copy any future changes
		args arg3 = args::copy;

		virtual bool match_group(const std::vector<args>& arg_v) override {
			bool base_match = base_group::match_group(arg_v);

			if (base_match == false) {
				return base_match;
			}

			std::size_t no_deletes_count = 0;
			std::size_t watch_count = 0;
			std::size_t copy_count = 0;
			for (auto arg : arg_v) {
				if (arg == arg1) {
					no_deletes_count++;
				}
				if (arg == arg2) {
					watch_count++;
				}
				if (arg == arg3) {
					copy_count++;
				}
			}

			return no_deletes_count == 1 and watch_count == 1 and copy_count == 1 and base_match == true;
		}
	};

	class group_5 : public group_3 ,public base_group {
	public:
		args arg1 = args::new_files_only;

		virtual bool match_group(const std::vector<args>& arg_v) override {
			bool base_match = base_group::match_group(arg_v);

			if (base_match == false) {
				return base_match;
			}

			base_match = group_3::match_group(arg_v);

			if (base_match == false) {
				return base_match;
			}
			
			std::size_t new_files_only_count = 0;
			for (auto arg : arg_v) {
				if (arg == arg1) {
					new_files_only_count++;
				}
			}

			return new_files_only_count == 1 and base_match == true;
		}

	};


	class arg_entry {
	public:
		std::vector<args> args_v;
		std::filesystem::path dst_p;
		std::filesystem::path src_p;
		std::size_t entry_number;
	};

	enum class file_action {
		copy,
		uninit = 1,
		delete_dst,
		modified,
		previous_name,
		new_name,
		unknown
	};

	enum class directory_completed_action {
		recursive_copy,
		uninit = 1,
		delete_all,
		previous_name,
		new_name,
		unknown,
	};

	class directory_info {
	public:
		std::filesystem::path p;
		std::uintmax_t number_of_files;
		directory_completed_action action;

		// Equality operator for unordered_set
		bool operator==(const directory_info& other) const {
			return p == other.p && number_of_files == other.number_of_files;
		}
	};

	class file_entry {
	public:
		directory_completed_action completed_action;
		file_action action;
		std::filesystem::path dst_p;
		std::filesystem::path src_p;
		std::filesystem::file_status s;
		std::unordered_set<directory_info>* p_di_set;
	};
}

// Hash specialization for directory_info
namespace std {
	template<>
	struct hash<core::directory_info> {
		std::size_t operator()(const core::directory_info& d) const noexcept {
			std::size_t h1 = std::hash<std::filesystem::path>{}(d.p);
			std::size_t h2 = std::hash<std::uintmax_t>{}(d.number_of_files);
			return h1 ^ (h2 << 1);
		}
	};
}