#ifndef LOG_H
#define LOG_H

#if ANDROMEDA_INTERNAL
#	include <spdlog/spdlog.h>
#   define ENGINE_LOGGER_NAME "AndromedaEngine"
#endif

namespace ENGINE_NS {
	void print(const std::string& message);
	void warn(const std::string& message);
	void error(const std::string& message);
} // namespace ENGINE_NS

#if ANDROMEDA_INTERNAL
#	if ANDROMEDA_DEBUG
#		define ANDROMEDA_VERBOSE(...) spdlog::trace(__VA_ARGS__)
#		define ANDROMEDA_INFO(...) spdlog::info(__VA_ARGS__)
#	else
#		define ANDROMEDA_VERBOSE(...) ((void)0)
#		define ANDROMEDA_INFO(...) ((void)0)
#	endif

#	define ANDROMEDA_WARN(...) spdlog::warn(__VA_ARGS__)
#	define ANDROMEDA_ERR(...) spdlog::error(__VA_ARGS__)
#endif

#endif