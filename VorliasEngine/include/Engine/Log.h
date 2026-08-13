#ifndef LOG_H
#define LOG_H
#include <format>
#include "spdlog/spdlog.h"

#if ANDROMEDA_INTERNAL
#	define ENGINE_LOGGER_NAME "AndromedaEngine"
#endif

namespace ENGINE_NS {
	void trace(const std::string& message);

	void print(const std::string& message);

	template<typename... Args>
	void print(std::format_string<Args...> fmt, Args&&... args);

	void warn(const std::string& message);
	void error(const std::string& message);
} // namespace ENGINE_NS

// #if ANDROMEDA_INTERNAL
// #	if ANDROMEDA_DEBUG
// #		define ANDROMEDA_VERBOSE(...) spdlog::trace(__VA_ARGS__)
// #		define ANDROMEDA_INFO(...) spdlog::info(__VA_ARGS__)
// #	else
// #		define ANDROMEDA_VERBOSE(...) ((void)0)
// #		define ANDROMEDA_INFO(...) ((void)0)
// #	endif

// #	define ANDROMEDA_WARN(...) spdlog::warn(__VA_ARGS__)
// #	define ANDROMEDA_ERR(...) spdlog::error(__VA_ARGS__)
// #endif

#define ANDROMEDA_VRB(...) spdlog::trace(__VA_ARGS__)
#define ANDROMEDA_LOG(...) spdlog::info(__VA_ARGS__)
#define ANDROMEDA_WRN(...) spdlog::warn(__VA_ARGS__)
#define ANDROMEDA_ERR(...) spdlog::error(__VA_ARGS__)

#define ANDROMEDA_VK_CHECK(cond, ...) \
	{ \
		if (!(cond)) { \
			ANDROMEDA_ERR(__VA_ARGS__); \
			return false; \
		} \
	}
#endif