#pragma once
#include <filesystem>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

namespace andromeda {
	enum class FileStatus {
		Added,
		Modified,
		Removed,
	};

	class FileWatcher {
	public:
		FileWatcher() : m_path() {}
		FileWatcher(std::filesystem::path path, std::chrono::duration<int, std::milli> delay) : m_path(path), m_delay(delay) {}

		bool Watch(const std::function<void(std::filesystem::path, FileStatus)>& action) {
			trace("Start watching on path '{}'", m_path.string());

			if (m_path.empty())
				return false;
			m_running = true;

			m_thread = std::thread([&, action]() {
				while (m_running) {
					auto it = m_paths.begin();
					while (it != m_paths.end()) {
						if (!std::filesystem::exists(it->first)) {
							action(it->first, FileStatus::Removed);
							it = m_paths.erase(it);
						} else {
							it++;
						}
					}
					for (auto& file : std::filesystem::recursive_directory_iterator(m_path)) {
						auto current_file_last_write_time = std::filesystem::last_write_time(file);
						// File creation
						if (!m_paths.contains(file.path())) {
							m_paths[file.path().string()] = current_file_last_write_time;
							action(file.path(), FileStatus::Added);
							// File modification

						} else {
							if (m_paths[file.path()] != current_file_last_write_time) {
								m_paths[file.path()] = current_file_last_write_time;
								action(file.path(), FileStatus::Modified);
							}
						}
					}

					std::this_thread::sleep_for(m_delay);
				}
			});

			return true;
		}

		void Stop() {
			m_running = false;

			if (m_thread.joinable())
				m_thread.join(); // wait for thread to die

			trace("Shut down file watcher");
		}

	private:
		bool m_running = false;
		std::filesystem::path m_path;
		std::unordered_map<std::string, std::filesystem::file_time_type> m_paths;
		std::chrono::duration<int, std::milli> m_delay;
		std::thread m_thread;
	};
} // namespace andromeda