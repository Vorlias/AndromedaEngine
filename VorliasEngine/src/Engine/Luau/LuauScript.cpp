#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/LuauState.h"
#include <luacode.h>
#include <memory>
#include <cstring>
#include <string>
#include <lualib.h>

typedef unsigned char byte;

using namespace ENGINE_NS;

static LuauState* s_compilerState;
static LuauCompileResult s_result;

bool LuauScript::CompileSource(char* source, int source_len, char* file_name, int file_name_len, int optimization_level) {
    if (s_compilerState == nullptr) {
		s_compilerState = new LuauState(LuauStateContext::Compiler);
	}
	
	auto L = s_compilerState->GetLuaState();

	lua_CompileOptions options{
		.optimizationLevel = optimization_level,
		.debugLevel = 1,
		.typeInfoLevel = 0,
	};

	size_t bytecode_size;
	char* bytecode = luau_compile(source, source_len, &options, &bytecode_size);
	int res = luau_load(L, "=check", bytecode, bytecode_size, 0);

	if (res != 0) {
		size_t len;
		const char* msg = lua_tolstring(L, -1, &len);
		lua_pop(L, 1);

		strcpy((char*)m_err, msg);
		m_errlen = len;
	} else {
		memcpy(m_bytecode, bytecode, bytecode_size);
        m_bytecodeSize = bytecode_size;
        m_errlen = 0;
	}

    free(bytecode);
    return res != 0;
}

LuauScript LuauScript::LoadFromMemory(std::string source, std::string name) {
	// LuauScript script;
	// script.CompileSource(source.data(), source.length(), name.data(), name.length(), 2);

	// return script;

}

LuauScript::LuauScript(): m_thread(nullptr)
{
}

bool LuauScript::LoadThread() {
    // auto main_thread = LuauState::gameState;
    // auto L = main_thread.GetLuaState();

    // m_thread = lua_newthread(L);
    // luaL_sandboxthread(m_thread);

	// int result = luau_load(m_thread, (std::string("=") + m_fileName).c_str(), static_cast<const char*>(m_bytecode), m_bytecodeSize, 0);

    // if (result != 0) {
	// 	// there was an error here
	// 	size_t error_len;
	// 	const char* error_message = lua_tolstring(m_thread, -1, &error_len);

	// 	std::string error(error_message, error_len);
	// 	lua_pop(m_thread, 1);

    //     m_thread = nullptr;
	// 	return false;
	// }

    // lua_pushstring(L, kThreads); // [-0, +1, -: 2]
    // lua_gettable(L, LUA_REGISTRYINDEX); // [-1, +1, -: 2]

    // lua_pushvalue(L, -2); // [+1, 3]
	// lua_pushvalue(L, -1); // [+1, 4]
    // lua_settable(L, -3); // [-2, 2]

    // lua_pop(L, 2); // [-2, 0]
    return false;
}

bool LuauScript::RunThread() {
    if (m_thread == nullptr) {
        if (!RunThread()) return false;
    }

    int status = lua_resume(m_thread, nullptr, 0);

	if (status == LUA_OK || status == LUA_YIELD)
		return true;

    lua_State* L = lua_mainthread(m_thread);

	if (const char* str = lua_tostring(m_thread, -1)) {
		// TODO: error
	} else {
		// TODO: unknown error
	}

    return false;
}

LuauScript::~LuauScript() {

}
