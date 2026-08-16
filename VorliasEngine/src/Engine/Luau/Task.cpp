#include "Engine/Luau/Task.h"
#include "Engine/Luau/Lib.h"
#include "Engine/Luau/ThreadData.h"
#include "Engine/Luau/PrettyPrint.h"
#include "Engine/Log.h"
#include "Engine/ObjectPool.h"
#include <lualib.h>
#include <vector>
using namespace andromeda_luau;

struct LuauScheduledData {
	float startTime{};
	float resumeTime{};
	int threadRef{};
	int argc{};
	int retc{};
	lua_Type* retv{nullptr};
};

constexpr float kMinDelayTime = 0.000001f;
constexpr const char* kScheduledThreads = "LuauScheduledThreads";

static andromeda::Pool<LuauScheduledData> s_scheduledDataPool(500, 50);

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

static int luaL_runthread(lua_State* L, lua_State* T, int argc, int* retc) {
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

		// if (lua_gettop(T) > 0 && retc != nullptr) {
		// 	std::cout << "top is " << lua_gettop(T) << ", " << luaL_typename(T, -1) << ":" << lua_tostring(T, -1) << "; "  << status << std::endl;
		// 	*retc = lua_gettop(T);
		// }

		// std::cout << "resume result is " << status << std::endl;

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

static LuauScheduledData* schedulethread(lua_State* L, lua_State* T, int argc, float resumeTime) {
	lua_rawgetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);

	int i;
	int n = lua_objlen(L, -1);
	for (i = 1; i <= n; i++) {
		lua_rawgeti(L, -1, i); // item = SCHEDULED_THREADS[i]

		LuauScheduledData* sd = static_cast<LuauScheduledData*>(lua_tolightuserdata(L, -1));
		float num = sd->resumeTime;

		lua_pop(L, 1);

		if (num > resumeTime) {
			break;
		}
	}

	LuauScheduledData* data = s_scheduledDataPool.acquire();
	data->startTime = now;
	data->resumeTime = resumeTime;
	data->argc = argc;

	lua_pushthread(T);
	data->threadRef = lua_ref(T, -1);
	lua_pop(T, 1);

	lua_pushlightuserdata(L, data);

	if (i > n) {
		// SCHEDULED_THREADS[#kScheduledThreads + 1] = item
		lua_rawseti(L, -2, i);
	} else {
		// Insert at 'i'; use table.insert for this:
		lua_getglobal(L, "table");
		lua_getfield(L, -1, "insert");

		lua_rawgetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);
		lua_pushinteger(L, i); // index
		lua_pushvalue(L, -5); // item

		lua_call(L, 3, 0); // table.insert(kScheduledThreads, i, item)

		lua_pop(L, 2); // pop 'table' library and item
	}

	lua_pop(L, 1); // pop kScheduledThreads
	return data;
}

static int task_spawn(lua_State* L) {
	int argc;
	lua_State* T = luaL_spawnthread(L, 1, &argc);

	int status = luaL_runthread(L, T, argc, nullptr);
	if (status == LUA_OK) {
		lua_settop(T, 0);
	}

	return 1;
}

static int task_delay(lua_State* L) {
	float time = static_cast<float>(luaL_checknumber(L, 1));

	int argc;
	lua_State* T = luaL_spawnthread(L, 2, &argc);

	schedulethread(L, T, argc, 0.0f);
	return 1;
}

static int task_wait(lua_State* L) {
	float delay_time = static_cast<float>(luaL_optnumber(L, 1, kMinDelayTime));
	if (delay_time <= kMinDelayTime) {
		delay_time = kMinDelayTime;
	}

	auto waitData = schedulethread(L, L, 0, now + delay_time);

	lua_pushnumber(L, delay_time);
	return lua_yield(L, 1);
}

static void runScheduledThreads(lua_State* L) {
	lua_rawgetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);

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

		if (data->resumeTime > now)
			break;

		if (lua_costatus(L, T) != LUA_COSUS)
			continue;

		int retc = 0;
		int status = luaL_runthread(L, T, data->argc, &retc);

		if (status == LUA_OK) {
			lua_settop(T, 0);
		}

		lua_unref(L, data->threadRef);
		s_scheduledDataPool.release(data);
	}

	// Remove threads that ran:
	int remove_count = i - 1;
	if (remove_count > 0) {
		lua_getglobal(L, "table");
		for (int j = 0; j < remove_count; j++) {
			lua_getfield(L, -1, "remove");
			lua_pushvalue(L, -3);
			lua_pushinteger(L, 1);
			lua_call(L, 2, 0); // table.remove(scheduled_thread_table, 1)
		}
		lua_pop(L, 1);
	}

	lua_pop(L, 1);
}

static void luaL_rundeferred(lua_State* L) {}


void andromeda_luau::runThreadScheduler(lua_State* L, float time) {
	now = time;
	runScheduledThreads(L);
}

const luaL_Reg taskLib[] = {
	{"spawn", task_spawn},
	{"delay", task_delay},
	{"wait", task_wait},
	{nullptr, nullptr},
};

void andromeda_luau::openTaskLib(lua_State* L) {
	luaL_registerlibrary(L, "task", taskLib, true);

	lua_newtable(L);
	lua_rawsetfield(L, LUA_REGISTRYINDEX, kScheduledThreads);
}