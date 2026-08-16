#pragma once
#include "LuauState.h"
#include "LuauScript.h"
#include "../Memory.h"
#include "../Common.h"
#include "../Application.h"
#include <chrono>
#include <thread>
#include <unordered_map>

namespace andromeda {
	class LuauRuntime {
	public:
		LuauRuntime();
		~LuauRuntime();
		void Update(float dt);

		Ref<LuauScript> LoadScriptFromFile(const std::string& fileName);
		bool ExecuteScript(Ref<LuauScript> script, LuauStateContext context = LuauStateContext::Game);
	private:
		float m_time = 0;
		LuauState* m_mainState;
		std::unordered_map<std::string_view, Ref<LuauScript>> m_scripts{};
		std::vector<std::unique_ptr<LuauScriptThread, LuauScriptThread::Cleanup>> m_scriptThreads{};
	};
} // namespace andromeda