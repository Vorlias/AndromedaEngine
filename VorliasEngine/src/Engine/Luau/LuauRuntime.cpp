#include "Engine/Luau/LuauRuntime.h"
#include "Engine/Luau/Task.h"
#include "Engine/File.h"
#include "Engine/Log.h"

andromeda::LuauRuntime::LuauRuntime() : m_mainState(LuauState::GetMainThread(LuauStateContext::Game)) {}

void andromeda::LuauRuntime::Update(float dt) {
	m_time += dt;
	andromeda_luau::runThreadScheduler(m_mainState->GetLuaState(), m_time);
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


bool andromeda::LuauRuntime::ExecuteScript(andromeda::Ref<andromeda::LuauScript> script, LuauStateContext context) {
	if (script == nullptr)
		return false;

	std::unique_ptr<andromeda::LuauScriptThread, andromeda::LuauScriptThread::Cleanup> thread(
		new LuauScriptThread(script), andromeda::LuauScriptThread::Cleanup()
	);
	if (!thread->Run()) {
		thread.reset();
		return false;
	}

	m_scriptThreads.push_back(std::move(thread));
	return true;
}

andromeda::LuauRuntime::~LuauRuntime() {
	m_scripts.clear();
}