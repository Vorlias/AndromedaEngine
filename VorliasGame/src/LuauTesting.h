#include "Engine/Luau/LuauScript.h"
#include "Engine/Application.h"
#include "Engine/Luau/LuauRuntime.h"
#include "Engine/Objects/Component.h"
#include "Engine/Scene/Scene.h"
using namespace std::chrono_literals;

class LuauApplication : public andromeda::Application {
public:
    bool Initialize() override final {
        luau = LuauRuntime::GetGameRuntime();

        auto testScript = luau->LoadScriptFromFile("assets/scripts/test.luau");


        luauEntity = scene.CreateEntity();
        luauEntity.AddComponent<LuauScriptComponent>(testScript);
        

        // auto luaEntity2 = scene.CreateEntity();
        // luaEntity2.AddComponent<LuauScriptComponent>(testScript);

        scene.Initialize();

        killSoon = std::thread([=]() {
            std::this_thread::sleep_for(3s);
            scene.Shutdown();
        });
		return true;
	}

    void Update(float dt) override final {
        luau->Update(dt);
        scene.Update(dt);
    }

    void Shutdown() override final {
        
    }
private:
    bool dunnit = false;
    Scene scene;
    SharedRef<LuauRuntime> luau;
    std::thread killSoon;
    Entity luauEntity;
};