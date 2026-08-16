#include "Engine/Luau/LuauRuntime.h"
#include "Engine/Luau/Task.h"
#include "Engine/File.h"

andromeda::LuauRuntime::LuauRuntime() : m_mainState(LuauState::GetMainThread(LuauStateContext::Game)) {}

void andromeda::LuauRuntime::Update(float dt) {
	m_time += dt;
	andromeda_luau::runThreadScheduler(m_mainState->GetLuaState(), m_time);
}

andromeda::Ref<andromeda::LuauScript> andromeda::LuauRuntime::LoadScriptFromFile(const std::string& fileName) {
	if (m_scripts.contains(fileName)) {
		return m_scripts.at(fileName);
	}

	auto source = ReadFile(fileName);
	if (source.empty()) return nullptr;
	
	auto script = LuauScript::CreateScript(source, fileName);

	m_scripts.insert({ fileName, script });
	return script;
}

andromeda::LuauScriptThread andromeda::LuauRuntime::ExecuteScript(andromeda::Ref<andromeda::LuauScript> script, LuauStateContext context) {
	LuauScriptThread thread(script);
	thread.Run();
	return thread;
}