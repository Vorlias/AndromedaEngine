#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/LuauState.h"
#include "Engine/Luau/PrettyPrint.h"
#include "Engine/Luau/Task.h"
#include "Engine/Log.h"
#include "Engine/Luau/Lib.h"
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

LuauScript::LuauScript() : andromeda::Asset(AssetType::LuauScript, UUID{}, "") {}

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
	int result = luau_load(m_thread, (std::string("=") + m_script->GetFilePath()).c_str(), static_cast<const char*>(bytecode.data), bytecode.size, 0);

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

	lua_pop(M, 1); // [-2, 0]

	return true;
}

bool LuauScriptThread::Run() {
	using namespace andromeda;
	using namespace andromeda_luau;
	if (m_running) return false;

	if (m_thread == nullptr) {
		if (m_script->HasErrored()) {
			error("Thread failed to compile: {}", m_script->GetError());
		} else if (m_script->IsCompiled()) {
			if (m_script->GetFilePath().empty()) {
				error("Script has not had a thread created for it yet - use CreateThread()");
			} else {
				error("Script '{}' has not had a thread created for it yet - use CreateThread()", m_script->GetFilePath());
			}
		} else {
			error("No bytecode was loaded for script - use Compile()");
		}

		return false;
	}

	int status = lua_resume(m_thread, nullptr, 0);

	if (status == LUA_OK || status == LUA_YIELD)
	{
		m_running = true;
		return true;
	}

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
	return (LuauThreadStatus)status;
}

void LuauScriptThread::Close() {
	if (m_thread != nullptr && m_running) luaL_closethread(m_thread);
}

LuauScript::~LuauScript() {}

LuauScriptThread::~LuauScriptThread() {}

void LuauScriptComponent::SetScript(Ref<andromeda::LuauScript> script) {
	m_script = script;
}

void LuauScriptComponent::SetProperty(std::string_view property, LuauValue value) {
	ANDROMEDA_ASSERTM(m_thread != nullptr, "Thread access on component before thread was initialized");

	lua_State* L = m_thread->GetLuauState();
	int top = lua_gettop(L);

	switch (value.type) {
		case LuauValue::Bool:
			lua_pushboolean(L, value.data.i);
			lua_setfield(L, -2, property.data());
			break;
		case LuauValue::Float:
			lua_pushnumber(L, value.data.f);
			lua_setfield(L, -2, property.data());
			break;
		case LuauValue::String:
			lua_pushlstring(L, value.data.s, value.size);
			lua_setfield(L, -2, property.data());
			break;
	}

	ANDROMEDA_ASSERT(lua_gettop(L) == top); // ensure top matches at end
}

void LuauScriptComponent::SetEnabled(bool enabled) {}


constexpr const char* kComponent = "LuauScriptComponent";
constexpr const char* kUpdateErr = "UpdateError";

constexpr const char* kAwake = "_init";
constexpr const char* kReady = "_ready";
constexpr const char* kUpdate = "_update";
constexpr const char* kDestroy = "_exit";

void LuauScriptComponent::Update(float dt) const {
	if (m_thread == nullptr) {
		andromeda::warn("Updating dead thread");
		return;
	}

	lua_State* L = *m_thread;
	int top = lua_gettop(L);

	lua_getfield(L, -1, kUpdate);
	if (lua_isfunction(L, -1)) {
		lua_State* T = lua_newthread(L);
		lua_xpush(L, T, -2); // function
		lua_xpush(L, T, -3); // table
		lua_pushnumber(T, dt); // deltaTime

		lua_pop(L, 1); // pop thread
		int status = luaL_runthread(L, T, 2);
	}

	lua_pop(L, 1);
	ANDROMEDA_ASSERT(lua_gettop(L) == top); // ensure top matches at end
}

void LuauScriptComponent::Start() {
	if (m_script == nullptr)
		return;
	if (m_start)
		return;

	if (!m_awake)
		Awake();

	lua_State* L = *m_thread;
	int top = lua_gettop(L);
	{
		luaL_checktype(L, -1, LUA_TTABLE);
		lua_getfield(L, -1, kReady);

		if (lua_isfunction(L, -1)) {
			lua_State* T = lua_newthread(L);
			lua_xpush(L, T, -2); // function
			lua_xpush(L, T, -3); // table

			lua_pop(L, 1); // pop thread
			(void)luaL_runthread(L, T, 1);
		}

		lua_pop(L, 1); // pop value
	}
	ANDROMEDA_ASSERT(lua_gettop(L) == top); // ensure top matches at end
	m_start = true;
	m_state = STATE_STARTED;
}

void LuauScriptComponent::Reset() {
	m_start = false;
	m_awake = false;
	m_err = false;

	if (m_thread != nullptr) m_thread->Close();
	m_thread.reset();
	m_state = STATE_ASLEEP;
}

void LuauScriptComponent::Shutdown() {
	if (m_thread == nullptr)
		return;

	lua_State* L = *m_thread;
	if (L == nullptr)
		return;

	int top = lua_gettop(L);
	{
		luaL_checktype(L, -1, LUA_TTABLE);
		lua_getfield(L, -1, kDestroy);

		if (lua_isfunction(L, -1)) {
			lua_State* T = lua_newthread(L);
			lua_xpush(L, T, -2); // function
			lua_xpush(L, T, -3); // table

			lua_pop(L, 1); // pop thread
			(void)luaL_runthread(L, T, 1);
		}

		lua_pop(L, 1); // pop value
	}

	ANDROMEDA_ASSERT(lua_gettop(L) == top); // ensure top matches at end
	m_entity.RemoveComponent<andromeda::LuauScriptComponent::UpdateLifecycle>(); // tag this as updateable

	m_thread->Close();
	m_thread.reset();
	m_state = STATE_CLOSED;
}

void LuauScriptComponent::Awake() {
	if (m_awake)
		return;
	if (m_script == nullptr)
		return;

	std::unique_ptr<andromeda::LuauScriptThread, andromeda::LuauScriptThread::Cleanup> thread(
		new LuauScriptThread(m_script), andromeda::LuauScriptThread::Cleanup()
	);

	// Set any component-specific stuff here
	{
		lua_State* L = *thread;
		int top = lua_gettop(L);

		if (m_entity) {
			andromeda_luau::pushEntityHandle(L, m_entity.m_scene, m_entity.m_entity);
			lua_setglobal(L, "entity");
		}

		// TODO: Set appropriate globals here?

		ANDROMEDA_ASSERT(lua_gettop(L) == top); // ensure top matches at end
	}

	if (!thread->Run()) {
		thread.reset();
		m_err = true;
		return;
	}

	// we can awake once the script's ran
	{
		lua_State* L = *thread;
		int top = lua_gettop(L);

		try {
			if (lua_istable(L, -1)) {
				lua_pushvalue(L, -1);
				lua_rawsetfield(L, LUA_REGISTRYINDEX, kComponent);

				lua_getfield(L, -1, kAwake);

				if (lua_isfunction(L, -1)) { // if func
					lua_State* T = lua_newthread(L);
					lua_xpush(L, T, -2); // function
					lua_xpush(L, T, -3); // table

					lua_pop(L, 1); // pop thread
					int status = luaL_runthread(L, T, 1);
					if (status == LUA_YIELD) {
						andromeda::warn("Detected yield inside method {}", kAwake);
					}
				}

				lua_pop(L, 1); // pop value

				// check for _update
				lua_getfield(L, -1, kUpdate);
				if (lua_isfunction(L, -1)) {
					if (!m_entity.HasComponent<andromeda::LuauScriptComponent::UpdateLifecycle>())
						m_entity.AddComponent<andromeda::LuauScriptComponent::UpdateLifecycle>(); // tag this as updateable
				}

				lua_pop(L, 1);
			} else {
				andromeda::error("Return value expected table got {}", luaL_typename(L, -1));
			}
		} catch (std::exception& e) {
			andromeda::error("{}", e.what());
			lua_pop(L, lua_gettop(L) - top);
			m_err = true;
			return;
		}

		ANDROMEDA_ASSERT(lua_gettop(L) == top); // ensure top matches at end
	}

	m_thread = std::move(thread);
	m_awake = true;
	m_state = STATE_AWAKE;
}