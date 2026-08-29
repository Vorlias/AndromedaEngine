#pragma once
#include <Andromeda.h>

namespace andromeda {
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
			}
		}

		std::filesystem::path ProjectPath{};

	private:
		std::vector<std::string> m_args;
	};
} // namespace andromeda