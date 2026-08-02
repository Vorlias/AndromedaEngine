#include "Engine/Log.h"
#include "spdlog/spdlog.h"

namespace ENGINE_NS {
	void print(const std::string& message) {
		spdlog::info(message);
	}

	void warn(const std::string& message) {
		spdlog::warn(message);
	}

	void error(const std::string& message) {
		spdlog::error(message);
	}
} // namespace ENGINE_NS
