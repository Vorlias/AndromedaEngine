#pragma once
#include "lua.h"
#include <unordered_map>

constexpr const char* kLuauState = "LuauStates";
constexpr const char* kThreads = "LuauThreads";
constexpr const char* kPinnedThreads = "LuauPinnedThreads";
constexpr const char* kRequires = "LuauRequires";

namespace ENGINE_NS {
	enum class LuauStateContext {
		Game = 1 << 0,

		Compiler = 10000,
	};

	class LuauState {
		static std::unordered_map<LuauStateContext, LuauState*> s_states;

	public:
		LuauState(LuauStateContext context);
		~LuauState();

		void SetGlobal(const std::string& name, lua_CFunction f);

		lua_State* GetLuaState();
		inline bool IsValid() {
			return L != nullptr;
		}

		static LuauState* GetLuauState(lua_State* L);
		static LuauState* GetMainThread(LuauStateContext context);

	private:
		lua_State* L;
		LuauStateContext m_context;
	};
} // namespace ENGINE_NS