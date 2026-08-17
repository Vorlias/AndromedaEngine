#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/LuauState.h"
#include "Engine/Luau/PrettyPrint.h"
#include "Engine/Log.h"
#include <luacode.h>
#include <memory>
#include <cstring>
#include <string>
#include <lualib.h>
#include <Luau/Bytecode.h>

typedef unsigned char byte;

using namespace ENGINE_NS;

static LuauState* s_compilerState;

const LuauBytecodeVersion LuauScript::GetBytecodeVersion() {
	return LuauBytecodeVersion{
		.min = LBC_VERSION_MIN,
		.max = LBC_VERSION_MAX,
		.target = LBC_VERSION_TARGET,
	};
}

const LuauBytecode LuauScript::GetBytecode() const {
	return LuauBytecode{
		.version = GetBytecodeVersion(),
		.size = m_bytecodeSize,
		.data = (bytecode_t)m_bytecode,
	};
}

bool LuauScript::CompileSource(const char* source, int source_len, const char* file_name, int file_name_len, int optimization_level) {
	if (s_compilerState == nullptr) {
		s_compilerState = new LuauState(LuauStateContext::Compiler);
	}

	auto L = s_compilerState->GetLuaState();

	lua_CompileOptions options{
		.optimizationLevel = optimization_level,
		.debugLevel = 1,
		.typeInfoLevel = 0,
	};

	options.vectorLib = "Vector3";
	options.vectorType = "Vector3";
	options.vectorCtor = "new";

	size_t bytecode_size;
	char* bytecode = luau_compile(source, source_len, &options, &bytecode_size);
	int res = luau_load(L, "=check", bytecode, bytecode_size, 0);

	if (res != 0) {
		size_t len;
		const char* msg = lua_tolstring(L, -1, &len);
		lua_pop(L, 1);

		strcpy((char*)m_err, msg);
		m_errlen = len;

		andromeda::error("Failed to compile: {}", m_err);
	} else {
		memcpy(m_bytecode, bytecode, bytecode_size);
		m_bytecodeSize = bytecode_size;
		m_errlen = 0;
		andromeda::trace("Compiled Luau script with {} bytes", m_bytecodeSize);
	}

	free(bytecode);
	return res == 0;
}

LuauScript::LuauScript() {}

bool LuauScriptThread::Create() {
	if (m_thread != nullptr)
		return false;

	if (!m_script->IsCompiled()) {
		return false;
	}

	auto main_thread = LuauState::GetMainThread(LuauStateContext::Game);
	auto M = main_thread->GetLuaState();

	m_thread = lua_newthread(M);
	luaL_sandboxthread(m_thread);

	LuauBytecode bytecode = m_script->GetBytecode();
	int result = luau_load(m_thread, (std::string("=") + m_script->GetFileName()).c_str(), static_cast<const char*>(bytecode.data), bytecode.size, 0);

	if (result != 0) {
		// there was an error here
		size_t error_len;
		const char* error_message = lua_tolstring(m_thread, -1, &error_len);

		std::string error(error_message, error_len);
		lua_pop(m_thread, 1);

		andromeda::error(error);
		m_thread = nullptr;
		return false;
	}

	lua_pushstring(M, kThreads); // [-0, +1, -: 2]
	lua_gettable(M, LUA_REGISTRYINDEX); // [-1, +1, -: 2]

	lua_pushvalue(M, -2); // [+1, 3]
	lua_pushvalue(M, -1); // [+1, 4]
	lua_settable(M, -3); // [-2, 2]

	lua_pop(M, 2); // [-2, 0]

	return true;
}

bool LuauScriptThread::Run() {
	using namespace andromeda;
	using namespace andromeda_luau;

	if (m_thread == nullptr) {
		if (m_script->HasErrored()) {
			error("Thread failed to compile: {}", m_script->GetError());
		} else if (m_script->IsCompiled()) {
			if (m_script->GetFileName().empty()) {
				error("Script has not had a thread created for it yet - use CreateThread()");
			} else {
				error("Script '{}' has not had a thread created for it yet - use CreateThread()", m_script->GetFileName());
			}
		} else {
			error("No bytecode was loaded for script - use Compile()");
		}

		return false;
	}

	int status = lua_resume(m_thread, nullptr, 0);

	if (status == LUA_OK || status == LUA_YIELD)
		return true;

	lua_State* L = lua_mainthread(m_thread);

	if (const char* errorMessage = lua_tostring(m_thread, -1)) {
		error(errorMessage);
	} else if (lua_istable(m_thread, -1)) {
		size_t len;
		const char* prettyErrorMessage = luaL_toprettylstring(m_thread, -1, &len);
		error(std::string(prettyErrorMessage, len));
	} else {
		// TODO: unknown error
		error("Unknown error");
	}

	return false;
}

LuauThreadStatus LuauScriptThread::GetThreadStatus() const {
	int status = lua_costatus(lua_mainthread(m_thread), m_thread);
	return (LuauThreadStatus) status;
}

void LuauScriptThread::Reset() {
	lua_resetthread(m_thread);
}

LuauScript::~LuauScript() {}

LuauScriptThread::~LuauScriptThread() {}