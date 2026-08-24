#include <lualib.h>
#include <spdlog/spdlog.h>
#include <stacktrace>
#include <iostream>
#include "Engine/Log.h"
#include "Engine/Luau/Task.h"
#include "Engine/Luau/ThreadData.h"
#include "Engine/Luau/LuauState.h"
#include "Engine/Luau/PrettyPrint.h"
#include "Engine/Data/Vector.h"
#include "Engine/Luau/AtomsDef.h"
#include "Engine/Objects/Object.h"

#include "Engine/Luau/Userdata.h"
#include "Engine/Luau/ComponentUserdata.h"
#include "Engine/Objects/Component.h"

using namespace andromeda;

static std::string lua_stack_tostring(lua_State* L) {
	std::string result;

	int n = lua_gettop(L); // Number of arguments

	size_t l;
	const char* s;
	bool should_pop = false;

	for (int i = 1; i <= n; i++) {
		if (i > 1) {
			result += ' ';
		}

		if (lua_istable(L, i)) {
			s = andromeda_luau::luaL_toprettylstring(L, i, &l);
		} else {
			s = luaL_tolstring(L, i, &l);
		}


		result += std::string(s, l);

		if (should_pop) {
			lua_pop(L, 1);
		}
	}

	return result;
}

int luau_print(lua_State* L) {
	luaL_where(L, 1);
	lua_insert(L, 1);

	andromeda::print(lua_stack_tostring(L));
	return 0;
}

int luau_warn(lua_State* L) {
	luaL_where(L, 1);
	lua_insert(L, 1);

	andromeda::warn(lua_stack_tostring(L));
	return 0;
}

// andromeda::LuauRequireHandler andromeda::LuauState::requireHandler = nullptr;
int luau_require(lua_State* L){
	auto context = LuauState::GetContextFromState(L);
	auto mainState = LuauState::GetMainThread(context);
	auto require = mainState->GetRequireHandler();

	if (require == nullptr) {
		luaL_error(L, "Cannot call require here");
		return 0;
	}


	int top = lua_gettop(L);
	luaL_checktype(L, 1, LUA_TSTRING);

	size_t len;
	const char* path = lua_tolstring(L, 1, &len);

	int ret = require(L, std::string_view(path, len));

	ANDROMEDA_ASSERT(top + ret == lua_gettop(L));
	return ret;
}

int luau_error(lua_State* L) {
	using namespace andromeda_luau;

	int level = luaL_optinteger(L, 2, 1);
	lua_settop(L, 1);

	if (!lua_isstring(L, 1)) {
		if (lua_istable(L, 1)) {
			luaL_pushprettystring(L, 1);
			lua_insert(L, -2); // move the string to the table position
		} else {
			(void)luaL_tolstring(L, 1, nullptr);
		}
		lua_settop(L, 1);
	}

	if (lua_isstring(L, 1) && level > 0) { // add extra information?
		luaL_where(L, level);
		lua_pushvalue(L, 1);
		lua_concat(L, 2);
	}

	lua_error(L);
}

static const luaL_Reg lua_globals[] = {
	{"print", luau_print},
	{"warn", luau_warn},
	{"error", luau_error},
	{nullptr, nullptr},
};

#include <debugging>
static void handleNewOrDestroyedThread(lua_State* parentThread, lua_State* thread) {
	using namespace andromeda_luau;

	if (parentThread == nullptr) {
		// thread destroyed
		LuauThreadData* td = static_cast<LuauThreadData*>(lua_getthreaddata(thread));
		return;
	}

	LuauThreadData* threadData = getThreadData(parentThread);
	if (threadData == nullptr) {
		return;
	}

	lua_setthreaddata(thread, threadData);
	LuauThreadData* td = getThreadData(thread);
	if (td != nullptr) {
		td->refCount++;
	}
}


static int16_t handleAtom(lua_State* L, const char* c_str, size_t len) {
	std::string_view sv(c_str, len);
	auto iter = s_LuauAtoms.find(sv);

	if (iter != s_LuauAtoms.end()) {
		return iter->second;
	}

	return -1;
}

LuauState::LuauState(LuauStateContext context) : m_context(context) {
	using namespace andromeda_luau;

	L = luaL_newstate();
	// Attach context to state
	lua_pushinteger(L, static_cast<int>(context));
	lua_setfield(L, LUA_REGISTRYINDEX, kContextId);

	// Open libraries
	luaL_openlibs(L);
	openTaskLib(L);

	registerVector2Lib(L);
	registerVector3Lib(L);
	registerObjectLib(L);
	RegisterComponents(L);

	// Protect core libraries and metatables from modification
	luaL_sandbox(L);

	// Create a new writable global table for current thread
	luaL_sandboxthread(L);

	// Add globals
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, lua_globals);
	lua_pop(L, 1);

	lua_pushcfunction(L, luau_require, "luau_require");
	lua_setglobal(L, "require");

	// add threads
	lua_pushstring(L, kThreads);
	lua_newtable(L);
	lua_settable(L, LUA_REGISTRYINDEX);


	// self reference
	lua_pushlightuserdata(L, this);
	lua_rawsetfield(L, LUA_REGISTRYINDEX, kLuauState);

	lua_Callbacks* cb = lua_callbacks(L);
	cb->userthread = handleNewOrDestroyedThread;
	cb->useratom = handleAtom;

	m_timeoutHandler = new LuauTimeoutHandler(L);
	// m_timeoutHandler->Start(); // TODO: Fix this
}

LuauState* LuauState::GetLuauState(lua_State* L) {
	if (L == nullptr) {
		return nullptr;
	}

	lua_rawgetfield(L, LUA_REGISTRYINDEX, kLuauState);
	LuauState* state = static_cast<LuauState*>(lua_tolightuserdata(L, -1));
	lua_pop(L, -1);
	return state;
}

LuauStateContext LuauState::GetContextFromState(lua_State* L) {
	lua_checkstack(L, 1);
	lua_rawgetfield(L, LUA_REGISTRYINDEX, kContextId);
	int contextNum = lua_tointeger(L, -1);
	lua_pop(L, 1);

	return static_cast<LuauStateContext>(contextNum);
}

std::unordered_map<LuauStateContext, LuauState*> LuauState::s_states;
LuauState* LuauState::GetMainThread(LuauStateContext context) {
	if (s_states.find(context) == s_states.end()) {
		LuauState* newState = new LuauState(context);
		s_states.insert({context, newState});
		return newState;
	}

	return s_states.at(context);
}

lua_State* LuauState::GetLuaState() {
	return L;
}

LuauState::~LuauState() {
	if (m_timeoutHandler != nullptr) {
		m_timeoutHandler->Stop();
		delete m_timeoutHandler;
	}

	lua_close(L);
}