#include <Andromeda.h>
#include "Engine/Main.h"

#include "Engine/FileDialogs.h"
#include "Engine/File.h"

#include "imgui/imgui.h"

using namespace andromeda;

class EditorApplication : public Application {
public:
	const WindowOptions GetWindowOptions() const override {
		return WindowOptions("Andromeda Editor", Vector2u(1024, 768), (WindowFlags::Maximized | WindowFlags::Resizable));
	}

	bool Initialize() override {
		CreateWindow(GetWindowOptions());

		InitIMGUI();
		ANDROMEDA_ASSERT(imgui != nullptr);

		return true;
	}

	void DrawIMGUI() override {
		ImGui::ShowDemoWindow();
	}
	
private:
	uint32_t wtf = 0;
};

Application* ApplicationMain(const ApplicationInit& ap) {
	if (!contains(ap.args, "-project")) {
		pfd::select_folder fold("Open Project", "", pfd::opt::force_path);
		auto result = fold.result();
		if (result.empty())
			return nullptr;
	}




	// if (result.empty())
	// 	return false;
	// SetDataPath(result);

	// auto andromedaProj = GetDataPath() / "project.andproj";
	// if (!std::filesystem::exists(andromedaProj)) {
	// 	warn("no project file");
	// }

	// auto assets = GetDataPath() / "assets";
	// if (!std::filesystem::exists(assets)) {
	// 	pfd::message(
	// 		"Invalid Andromeda project", "The path " + GetDataPath(true).string() + " contains no assets folder", pfd::choice::ok, pfd::icon::error
	// 	);
	// 	return false;
	// }

	return new EditorApplication();
}