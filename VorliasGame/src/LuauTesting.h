#include "Engine/Luau/LuauScript.h"
#include "Engine/Application.h"
#include "Engine/Luau/LuauRuntime.h"
#include "Engine/Objects/Component.h"
#include "Engine/Scene/Scene.h"

class LuauApplication : public andromeda::Application {
public:
    bool Initialize() override final {
        auto luauEntity = scene.CreateEntity();
        auto testScript = luau.LoadScriptFromFile("assets/scripts/test.luau");

        luauEntity.AddComponent<LuauScriptComponent>(testScript);

		return true;
	}

    void Update(float dt) override final {
        luau.Update(dt);
        scene.Update(dt);
    }
private:
    Scene scene;
    LuauRuntime luau;
};