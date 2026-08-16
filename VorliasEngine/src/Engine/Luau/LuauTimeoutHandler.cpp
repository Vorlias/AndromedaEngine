#include "Engine/Luau/LuauTimeoutHandler.h"

andromeda::ResetAllHandler& andromeda::LuauTimeoutHandler::resetAllHandler() {
	static ResetAllHandler handler = nullptr;
	return handler;
}

static const std::chrono::milliseconds kWatchdogInterval(250);
static void handleTimeoutInterrupt(lua_State* L, int gc) {
	andromeda::ResetAllHandler& reset_all = andromeda::LuauTimeoutHandler::resetAllHandler();
	if (reset_all != nullptr) {
		reset_all();
	}

	// Clear the interrupt before calling back into Luau, or else the interrupt
	// may get stuck in a recursive loop, causing a stack-overflow crash:
	lua_callbacks(L)->interrupt = nullptr;

	// Throw error:
	lua_checkstack(L, 1);
	luaL_error(L, "execution timed out");
}

int andromeda::LuauTimeoutHandler::s_defaultTimeout = 10;
andromeda::LuauTimeoutHandler::LuauTimeoutHandler(lua_State* L) : m_state(L), m_timeout(s_defaultTimeout), m_callbacks(lua_callbacks(L)) {
    ResetTimer();
}

void andromeda::LuauTimeoutHandler::RunTimeoutHandler() {
	while (m_running) {
		timepoint_t now = std::chrono::steady_clock::now();
		if (now > m_nextTimeout) {
			m_callbacks->interrupt = handleTimeoutInterrupt;
		}

		std::this_thread::sleep_for(kWatchdogInterval);
	}
}

void andromeda::LuauTimeoutHandler::ResetTimer() {
	m_nextTimeout = std::chrono::steady_clock::now() + m_timeout;
	m_callbacks->interrupt = nullptr;
}

void andromeda::LuauTimeoutHandler::Start() {
	if (m_running)
		return;
	m_running = true;
	m_thread = std::thread(&LuauTimeoutHandler::RunTimeoutHandler, this);
}

void andromeda::LuauTimeoutHandler::Stop() {
	if (!m_running)
		return;

	m_running = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
}

