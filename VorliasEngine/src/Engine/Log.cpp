#include "Engine/Log.h"
#include "spdlog/spdlog.h"
#include "Engine/File.h"
#include "Engine/Platform.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <print>

#define DEFAULT_LOGGER_NAME "AndromedaEngine"
namespace andromeda {
	static std::shared_ptr<spdlog::logger> logger;
	static bool init = false;

	std::shared_ptr<spdlog::logger> getLogger() {
		return logger;
	}

	void initializeLogger(const std::string& logPath) {
		if (init) return;
		init = true;

		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^%v%$");
		consoleSink->set_level(spdlog::level::info);

		if (std::filesystem::exists(logPath)) {
			std::filesystem::path newPath = logPath;
			newPath.replace_filename(newPath.stem().string() + "-prev" + newPath.extension().string());
			std::filesystem::rename(logPath, newPath);
		}

		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);
		fileSink->set_level(spdlog::level::trace);
		fileSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %l: %v%$");

		std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};

		logger = std::make_shared<spdlog::logger>(DEFAULT_LOGGER_NAME, sinks.begin(), sinks.end());
#if ANDROMEDA_DEBUG
		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);
#else
		logger->set_level(spdlog::level::info);
		logger->flush_on(spdlog::level::info);
#endif

		spdlog::register_logger(logger);
		print("Created log file at " + logPath);
	}

	void trace(const std::string& message) {
		if (logger)
			logger->trace(message);
		else
			std::cout << COLOR_GRAY << "[VRB] " <<  message << COLOR_RESET << std::endl;
	}

	void print(const std::string& message) {
		if (logger)
			logger->info(message);
		else {
			std::cout << COLOR_GREEN << "[INF] " <<  message << COLOR_RESET << std::endl;
		}
	}

	void warn(const std::string& message) {
		if (logger)
			logger->warn(message);
		else {
			std::cout << COLOR_YELLOW << "[WRN] " <<  message << COLOR_RESET << std::endl;
		}
	}

	void error(const std::string& message) {
		if (logger)
			logger->error(message);
		else {
			std::cout << COLOR_RED << "[ERR] " <<  message << COLOR_RESET << std::endl;
		}
	}
} // namespace andromeda
