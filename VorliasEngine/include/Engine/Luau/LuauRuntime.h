#pragma once
#include "LuauState.h"
#include "LuauScript.h"
#include "../Memory.h"
#include <chrono>
#include <thread>
#include <unordered_map>

namespace andromeda {
	class LuauRuntime {
	public:
		LuauRuntime();
		void Update(float dt);

		Ref<LuauScript> LoadScriptFromFile(const std::string& fileName);
		LuauScriptThread ExecuteScript(Ref<LuauScript> script, LuauStateContext context = LuauStateContext::Game);
	private:
		float m_time = 0;
		LuauState* m_mainState;
		std::unordered_map<std::string_view, Ref<LuauScript>> m_scripts;
	};
} // namespace andromeda