#include "Engine/Luau/Task.h"
#include "Engine/Luau/Lib.h"
#include "Engine/Luau/ThreadData.h"
#include "Engine/Luau/PrettyPrint.h"
#include "Engine/Log.h"
#include <lualib.h>
using namespace andromeda_luau;

struct LuauScheduledData {
	float startTime;
	float resumeTime;
	int threadRef;
	int argc;
};

constexpr float kMinDelayTime = 0.000001f;
constexpr const char* kScheduledThreads = "LuauScheduledThreads";

static float now = 0.0f;

lua_State* luaL_spawnthread(lua_State* L, int idx, int* argc) {
	bool isFunction = lua_isfunction(L, idx);
	bool isThread = lua_isthread(L, idx);

	if (!isFunction && !isThread) {
		luaL_errorL(L, "expected function or thread; got %s", luaL_typename(L, idx));
	}

	int offset = idx - 1;
	int args = lua_gettop(L) - offset;

	lua_State* T;
	if (isThread) {
		T = lua_tothread(L, idx);
		lua_pushvalue(L, idx);
	} else {
		T = lua_newthread(L);
	}

	for (int i = (isThread ? 2 : 1) + offset; i - offset <= args; i++) {
		lua_xpush(L, T, i);
	}

	*argc = args - 1;
	return T;
}

int luaL_runthread(lua_State* L, lua_State* T, int argc) {
	bool alive = true;

	void* threadDataPtr = lua_getthreaddata(T);
	if (threadDataPtr != nullptr) {
		LuauThreadData* threadData = static_cast<LuauThreadData*>(threadDataPtr);
		alive = threadData->alive;
	}

	if (!alive) {
		return LUA_ERRRUN;
	}

	lua_pushthread(T);
	lua_xmove(T, L, 1);

	int threadIdx = lua_gettop(L);
	int status = lua_resume(T, L, argc);

	if (status == LUA_OK || status == LUA_YIELD) {
		lua_remove(L, threadIdx);
		return status;
	}

	std::string error;
	if (const char* str = lua_tostring(T, -1)) {
		error += str;
	} else {
		error += "unknown error";
	}

	lua_remove(L, threadIdx);
	andromeda::error(error);
	return status;
}

static void luaL_schedulethread(lua_State* L, lua_State* T, int argc, float resumeTime) {
	lua_rawgetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);

	int i;
	int n = lua_objlen(L, -1);
}

static int task_spawn(lua_State* L) {
	int argc;
	lua_State* T = luaL_spawnthread(L, 1, &argc);

	int status = luaL_runthread(L, T, argc);
	if (status == LUA_OK) {
		lua_settop(T, 0);
	}

	return 1;
}

static int task_delay(lua_State* L) {
	float time = static_cast<float>(luaL_checknumber(L, 1));

	int argc;
	lua_State* T = luaL_spawnthread(L, 2, &argc);

	luaL_schedulethread(L, T, argc, 0.0f);
	return 1;
}

static int task_wait(lua_State* L) {
	float delay_time = static_cast<float>(luaL_optnumber(L, 1, kMinDelayTime));
	if (delay_time <= kMinDelayTime) {
		delay_time = kMinDelayTime;
	}

	luaL_schedulethread(L, L, 0, now + delay_time);
	return lua_yield(L, 0);
}

static void luaL_runscheduled(lua_State* L) {
	lua_rawgetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);
	luaL_debugstack(L);

	int numTasks = lua_objlen(L, -1);
	if (numTasks == 0) {
		lua_pop(L, 1);
		return;
	}

	int i;
	for (i = 1; i <= numTasks; i++) {
		lua_rawgeti(L, -1, i);

		LuauScheduledData* data = static_cast<LuauScheduledData*>(lua_tolightuserdata(L, -1));
		if (data == nullptr) {
			continue;
		}

		lua_getref(L, data->threadRef);
		lua_State* T = lua_tothread(L, -1);
		lua_pop(L, 2);

		if (data->resumeTime > now) break;
		if (lua_costatus(L, T) != LUA_COSUS) continue;

		int status = luaL_runthread(L, T, data->argc);
		if (status == LUA_OK) {
			lua_settop(T, 0);
		}

		lua_unref(L, data->threadRef);
	}
}

static void luaL_rundeferred(lua_State* L) {

}


int andromeda_luau::luaL_runscheduler(lua_State* L, float time) {
	now = time;
	luaL_runscheduled(L);
	// luaL_rundeferred(L);
}

const luaL_Reg taskLib[] = {
	{"spawn", task_spawn},
	{"delay", task_delay},
	{"wait", task_wait},
	{nullptr, nullptr},
};

void andromeda_luau::luaL_openTaskLib(lua_State* L) {
	luaL_registerlibrary(L, "task", taskLib, true);

	lua_newtable(L);
	lua_rawsetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);
}