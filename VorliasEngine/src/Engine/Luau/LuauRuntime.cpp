#include "Engine/Luau/LuauRuntime.h"
#include "Engine/Luau/LuauState.h"
#include "Engine/Luau/Task.h"
#include "Engine/File.h"
#include "Engine/Log.h"
using namespace andromeda;

SharedRef<LuauRuntime> LuauRuntime::s_gameRuntime = nullptr;
std::unordered_map<LuauStateContext, SharedRef<LuauRuntime>> LuauRuntime::s_runtimes{};


int require(lua_State* L, std::string_view path) {
	auto ctx = andromeda::LuauState::GetContextFromState(L);
	ANDROMEDA_ASSERT(ctx <= LuauStateContext::LAST);
	auto rt = andromeda::LuauRuntime::GetRuntimeFromContext(ctx);
	ANDROMEDA_ASSERT(rt != nullptr);

	luaL_error(L, "TODO %s", path.data());
	return 0;
}

andromeda::LuauRuntime::LuauRuntime(LuauStateContext context) : m_mainState(LuauState::GetMainThread(context)) {
	m_mainState->SetRequireHandler(require);
}

void andromeda::LuauRuntime::Update(float dt) {
	m_time += dt;
	andromeda_luau::runThreadScheduler(m_mainState->GetLuaState(), m_time);
}

const std::string LuauRuntime::ResolveModulePath(lua_State* L, std::string_view module) {
	
}

void LuauRuntime::PushModule(lua_State* L, std::string_view module) {

}

SharedRef<LuauRuntime> LuauRuntime::GetRuntimeFromContext(LuauStateContext context) {
	return s_runtimes.at(context);
}

andromeda::SharedRef<andromeda::LuauRuntime> andromeda::LuauRuntime::GetGameRuntime() {
	if (s_gameRuntime == nullptr) {
		s_gameRuntime = std::make_shared<andromeda::LuauRuntime>(LuauStateContext::Game);
		s_runtimes.insert({ LuauStateContext::Game, s_gameRuntime });
	}

	return s_gameRuntime;
}

andromeda::Ref<andromeda::LuauScript> andromeda::LuauRuntime::LoadScriptFromFile(const std::string& fileName) {
	if (m_scripts.contains(fileName)) {
		return m_scripts.at(fileName);
	}

	trace("Loading script from file {}", fileName);
	auto source = ReadFile(fileName);
	if (source.empty())
	{
		andromeda::warn("Could not load script at filePath {}", fileName);
		return nullptr;
	}


	auto script = LuauScript::CreateScript(source, fileName);

	m_scripts.insert({fileName, script});
	return script;
}

andromeda::LuauRuntime::~LuauRuntime() {
	m_scripts.clear();
}