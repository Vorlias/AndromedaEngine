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
	class LuauRuntime final {
	public:
		static SharedRef<LuauRuntime> GetGameRuntime();
		static SharedRef<LuauRuntime> GetRuntimeFromContext(LuauStateContext context);
	public:
		LuauRuntime(LuauStateContext context = LuauStateContext::Game);
		~LuauRuntime();

		void Update(float dt);

		Ref<LuauScript> LoadScriptFromFile(const std::string& fileName);

		const std::string ResolveModulePath(lua_State* L, std::string_view module);
		void PushModule(lua_State* L, std::string_view module);

		void Reset();
	private:
		static SharedRef<LuauRuntime> s_gameRuntime;
		static std::unordered_map<LuauStateContext, SharedRef<LuauRuntime>> s_runtimes;
	private:
		float m_time = 0;
		LuauState* m_mainState;
		std::unordered_map<std::string_view, Ref<LuauScript>> m_scripts{};
		std::unordered_map<std::string_view, std::shared_ptr<LuauScriptThread>> m_modules{};
	};
} // namespace andromeda