#pragma once
#include "imgui/imgui.h"
#include "Engine/Log.h"
#include "spdlog/spdlog.h"

#include "spdlog/details/null_mutex.h"
#include "spdlog/sinks/base_sink.h"
#include <memory>
#include <mutex>

namespace andromeda {
	struct Console;

	template<typename Mutex>
	class ConsoleSink : public spdlog::sinks::base_sink<Mutex> {
	public:
		ConsoleSink(Console* console) : m_console(console) {}

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;

		void flush_() override {}

	private:
		Console* m_console;
	};

	using my_sink_mt = ConsoleSink<std::mutex>;
	using my_sink_st = ConsoleSink<spdlog::details::null_mutex>;

	struct Console {
		struct Item {
			spdlog::level::level_enum level;
			std::string message;
		};

		std::vector<Item> items;
		bool open = true;
		bool scrollToBottom = false;
		bool autoScroll = true;
		ImFont* font{};

		Console() {

		};

		void Initialize(ImFont* font) {
			auto logger = andromeda::getLogger();

			auto sink = std::make_shared<my_sink_mt>(this);
			sink->set_pattern("[%H:%M:%S] %^%v%$");
			sink->set_level(spdlog::level::info);
			logger->sinks().push_back(sink);

			this->font = font;
		}

		void Log(spdlog::level::level_enum level, std::string message) {
			items.emplace_back(level, message);
		}

		void Draw();

		~Console() {
			// auto logger = andromeda::getLogger();

			// auto sinks = logger->sinks();
			// sinks.erase(std::find(sinks.begin(), sinks.end(), *this));
		}
	};
}; // namespace andromeda

template<typename Mutex>
void andromeda::ConsoleSink<Mutex>::sink_it_(const spdlog::details::log_msg& msg) {
	// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
	// msg.payload (before v1.3.0: msg.raw) contains pre formatted log

	// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
	spdlog::memory_buf_t formatted;
	spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
	// std::cout << fmt::to_string(formatted);

	m_console->Log(msg.level, fmt::to_string(formatted));
}
