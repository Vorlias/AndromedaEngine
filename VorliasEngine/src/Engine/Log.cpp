#include "Engine/Log.h"
#include "spdlog/spdlog.h"

namespace ENGINE_NS {
	void trace(const std::string& message) {
		spdlog::trace(message);
	}

	void print(const std::string& message) {
		spdlog::info(message);
	}

	template<typename... Args>
	void print(std::format_string<Args...> fmt, Args&&... args) {
		spdlog::info(fmt, std::forward<Args>(args)...);
	}

	void warn(const std::string& message) {
		spdlog::warn(message);
	}

	void error(const std::string& message) {
		spdlog::error(message);
	}
} // namespace ENGINE_NS
