#pragma once
#include <Andromeda.h>

template<typename C, typename V>
using Field = V(C::*);

namespace andromeda {
	template<typename T>
	struct ArgParam {
		std::string fullName{};
		char shortName{};
		T defaultValue{};

		ArgParam(std::string fullName) : fullName(fullName) {}
		ArgParam(std::string fullName, T defaultValue) : fullName(fullName), defaultValue(defaultValue) {}
		ArgParam(std::string fullName, char shortName) : fullName(fullName), shortName(shortName) {}
		ArgParam(std::string fullName, char shortName, T defaultValue) : fullName(fullName), shortName(shortName), defaultValue(defaultValue) {}
	};

	template<typename C, typename T>
	struct Arg {
		ArgParam<T> param;
		Field<C, T> field;
	};

	template<typename T>
	struct ArgBuilder {
		std::unordered_map<uint32_t, Arg<T, bool>> switches{};
		std::unordered_map<uint32_t, Arg<T, std::filesystem::path>> filePathArgs{};
		std::unordered_map<uint32_t, Arg<T, std::string>> stringArgs{};

		std::unordered_map<std::string, uint32_t> longNameMap{};
		std::unordered_map<char, uint32_t> shortNameMap{};

		ArgBuilder<T>& AddPathArgument(Field<T, std::filesystem::path> field, ArgParam<std::filesystem::path> param) {
			uint32_t nextId = ++argIdx;
			longNameMap.insert({param.fullName, nextId});
			if (param.shortName != 0) {
				shortNameMap.insert({param.shortName, nextId});
			}

			filePathArgs.insert({nextId, Arg<T, std::filesystem::path>{param, field}});
			return *this;
		}

		ArgBuilder<T>& AddStringArgument(Field<T, std::string> field, ArgParam<std::string> param) {
			return *this;
		}

		ArgBuilder<T>& AddFlag(Field<T, bool> field, std::string name, char shortName = 0) {
			return *this;
		}

		T Build(const std::vector<std::string> args) {
			T result;

			for (auto it = args.begin(); it != args.end(); ++it) {
				auto value = *it;
				if (value.starts_with("--")) {
					value = value.substr(2);
					
					longNameMap.contains(value);
				}
			}

			return result;
		}

	private:
		uint32_t argIdx{};
	};

	class EditorArgParser {
	public:
		EditorArgParser(const std::vector<std::string>& args) : m_args(args) {}
		void Parse() {
			for (auto it = m_args.begin(); it != m_args.end(); it++) {
				if (*it == "--project" || *it == "-P") {
					it++;
					if (it != m_args.end()) {
						ProjectPath = *it;
					} else {
						pfd::message("Invalid project path", "project argument requires a path to be supplied", pfd::choice::ok, pfd::icon::error);
					}
				}

				if (*it == "--run") {
					RunMode = true;
				}
			}
		}

		std::filesystem::path ProjectPath{};
		bool RunMode = false;

	private:
		std::vector<std::string> m_args;
	};
} // namespace andromeda