#include "Engine/Luau/ThreadData.h"

static void decrementThreadData(lua_State* L, andromeda_luau::LuauThreadData* threadData) {
	if (threadData == nullptr)
		return;

	threadData->refCount -= 1;
	if (threadData->refCount <= 0) {
		lua_setthreaddata(L, nullptr);
		delete threadData;
	}
}

void andromeda_luau::LuauThreadData::CreateRef(lua_State* L) {
	if (scriptThread != nullptr && scriptRef == LUA_REFNIL) {
		lua_pushthread(scriptThread);
		lua_xmove(scriptThread, L, 1);
		scriptRef = lua_ref(L, -1);
		lua_pop(L, 1);
	}

	if (workingThread != nullptr && workingRef == LUA_REFNIL) {
		lua_pushthread(workingThread);
		lua_xmove(workingThread, L, 1);
		workingRef = lua_ref(L, -1);
		lua_pop(L, 1);
	}
}

andromeda_luau::LuauThreadData* andromeda_luau::createThreadData(lua_State* T) {
	LuauThreadData* data = new LuauThreadData();
	data->alive = true;
	data->ownThread = T;
	data->scriptThread = nullptr;
	data->mainThread = lua_mainthread(T);
	data->scriptRef = LUA_REFNIL;
	data->refCount = 1;

	LuauThreadData* existingThreadData = static_cast<LuauThreadData*>(lua_getthreaddata(T));
	if (existingThreadData != nullptr) {
		decrementThreadData(T, existingThreadData);
	}

	lua_setthreaddata(T, data);
	return data;
}

andromeda_luau::LuauThreadData* andromeda_luau::getThreadData(lua_State* T) {
	void* threadDataPtr = lua_getthreaddata(T);
	if (threadDataPtr == nullptr)
		return nullptr;
	return static_cast<LuauThreadData*>(threadDataPtr);
}