#include <Andromeda.h>

using namespace std::chrono_literals;
using namespace andromeda;

class LuauApplication : public andromeda::Application {
public:
    bool Initialize() override final {
        luau = LuauRuntime::GetGameRuntime();

        auto parent = scene.CreateEntity();

        auto testScript = luau->LoadScriptFromFile("assets/scripts/test.luau");

        luauEntity = scene.CreateEntity();
        luauEntity.AddComponent<LuauScriptComponent>(testScript);
        luauEntity.SetParent(parent);
        
        scene.Initialize();

        killSoon = std::thread([=]() {
            std::this_thread::sleep_for(2s);
            // luauEntity.RemoveComponent<LuauScriptComponent>();
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