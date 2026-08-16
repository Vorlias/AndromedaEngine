#pragma once
#include <chrono>
#include "lualib.h"
#include <thread>

namespace andromeda {
	using ResetAllHandler = void (*)(void);
	using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;

	class LuauTimeoutHandler {
		static int s_defaultTimeout;

	public:
		static ResetAllHandler& resetAllHandler();

	private:
		void RunTimeoutHandler();

	public:
		LuauTimeoutHandler(lua_State* L);
		void Start();
		void Stop();
        void ResetTimer();

	private:
		lua_State* m_state;
		lua_Callbacks* m_callbacks;
		std::thread m_thread;
		bool m_running;

		timepoint_t m_nextTimeout;
		std::chrono::seconds m_timeout;
	};
} // namespace andromeda
