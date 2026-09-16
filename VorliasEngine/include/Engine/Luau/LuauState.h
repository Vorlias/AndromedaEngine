#pragma once
#include "lua.h"
#include "LuauTimeoutHandler.h"
#include <unordered_map>

constexpr const char* kContextId = "LuauContext";
constexpr const char* kLuauState = "LuauStates";
constexpr const char* kThreads = "LuauThreads";
constexpr const char* kPinnedThreads = "LuauPinnedThreads";
constexpr const char* kRequires = "LuauRequires";



namespace ENGINE_NS {
	typedef int(*LuauRequireHandler)(lua_State* L, std::string_view path);

	enum class LuauStateContext {
		None = 0,

		Game = 1 << 0,
		Editor = 1 << 1,

		ALL = Game | Editor,
		LAST = Editor,

		Compiler = 10000,
	};

	class LuauState {
		static std::unordered_map<LuauStateContext, LuauState*> s_states;

	public:
		LuauState(LuauStateContext context);
		~LuauState();

		lua_State* GetLuaState();
		inline bool IsValid() {
			return L != nullptr;
		}

		static LuauState* GetLuauState(lua_State* L);
		static LuauState* GetMainThread(LuauStateContext context);
		static LuauStateContext GetContextFromState(lua_State* L);
		
		ANDROMEDA_GETCONST LuauRequireHandler GetRequireHandler() const { return m_requireHandler; }
		ANDROMEDA_SETCONST void SetRequireHandler(LuauRequireHandler requireHandler) {
			m_requireHandler = requireHandler;
		}

		void Reset();
	private:
		void InitState();
	private:
		lua_State* L;
		LuauStateContext m_context;
		LuauTimeoutHandler* m_timeoutHandler;
		LuauRequireHandler m_requireHandler{};
	};
} // namespace ENGINE_NS