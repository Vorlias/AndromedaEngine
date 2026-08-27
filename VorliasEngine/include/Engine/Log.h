#ifndef LOG_H
#define LOG_H
#include <format>
#include "spdlog/spdlog.h"

#if ANDROMEDA_INTERNAL
#	define ENGINE_LOGGER_NAME "AndromedaEngine"
#endif

namespace andromeda {
	std::shared_ptr<spdlog::logger> getLogger();

	void initializeLogger(const std::string& logPath);

	void trace(const std::string& message);
	void print(const std::string& message);
	void warn(const std::string& message);
	void error(const std::string& message);

	template<typename... Args>
	void trace(std::format_string<Args...> fmt, Args&&... args) {
		trace(std::format(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	constexpr void print(std::format_string<Args...> fmt, Args&&... args) {
		print(std::format(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void warn(std::format_string<Args...> fmt, Args&&... args) {
		warn(std::format(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void error(std::format_string<Args...> fmt, Args&&... args) {
		error(std::format(fmt, std::forward<Args>(args)...));
	}
} // namespace andromeda


#if ANDROMEDA_DEBUG
#define ANDROMEDA_VRB(...) andromeda::trace(__VA_ARGS__)
#else
#define ANDROMEDA_VRB(...) ((void)0)
#endif

#define ANDROMEDA_LOG(...) andromeda::print(__VA_ARGS__)
#define ANDROMEDA_WRN(...) andromeda::warn(__VA_ARGS__)
#define ANDROMEDA_ERR(...) andromeda::error(__VA_ARGS__)

#define ANDROMEDA_VK_CHECK(cond, ...) \
	{ \
		if (!(cond)) { \
			ANDROMEDA_ERR(__VA_ARGS__); \
			return false; \
		} \
	}

#define ANDROMEDA_VK_ASSERT(cond, ...) ANDROMEDA_ASSERTM(cond == VK_SUCCESS, __VA_ARGS__)
#endif