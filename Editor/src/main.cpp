#include <Andromeda.h>
#include "Engine/Main.h"
#include "Engine/FileDialogs.h"
#include "Engine/File.h"
#include "imgui/imgui.h"
#include <SDL3/SDL_surface.h>

#include "Widgets/IconsLucide.h"

#include "EditorPanels/SceneHeirarchy.h"
#include "EditorPanels/Inspector.h"

#include "AndromedaEditorIcon.h"

#include "Widgets/TextEditor.h"

#include "EditorPanels/Console.h"


using namespace andromeda;

static WindowIcon s_editorWindowIcon(AndromedaIcon, AndromedaIcon_len);

class EditorApplication : public Application {
public:
	EditorApplication(const std::filesystem::path& projectPath)
		: m_projectRootPath(projectPath), m_activeScene(nullptr), m_sceneHierarchyPanel(m_activeScene), m_inspector(m_activeScene) {
		std::cout << "root path is " << m_projectRootPath.string() << std::endl;
	}

	const WindowOptions& GetWindowOptions() const override {
		WindowOptions options("Andromeda Editor", Vector2u(1024, 768), (WindowFlags::Maximized | WindowFlags::Resizable));
		options.windowIcon = s_editorWindowIcon; // WindowIcon(AndromedaIcon, AndromedaIcon_len);

		return options;
	}

	bool Initialize() override {
		SetDataPath(m_projectRootPath / "data");
		trace("Set data path to {}", GetDataPath(true).string());



		CreateWindow(GetWindowOptions());
		std::string title;

		title += "Andromeda Project - ";

		title += ANDROMEDA_VERSION_STRING;
		title += " <" + Engine::GetInstance().GetRenderer()->GetAPIString() + ">";

		GetMainWindow()->SetTitle(title.c_str());

		InitIMGUI();
		ANDROMEDA_ASSERT(imgui != nullptr);

		auto& io = ImGui::GetIO();

		io.Fonts->AddFontDefault();
		auto imFont = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf", 15.0f);


		io.FontDefault = imFont;
		{
			static const ImWchar icons_ranges[] = {ICON_MIN_LC, ICON_MAX_16_LC, 0};
			ImFontConfig icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.GlyphMinAdvanceX = 20.0f * 2.0f / 3.0f;

			io.Fonts->AddFontFromFileTTF("assets/fonts/" FONT_ICON_FILE_NAME_LC, 20.0f * 2.0f / 3.0f, &icons_config, icons_ranges);
		}

		textEditorFont = io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-Regular.ttf", 15.0f);

        m_luau = LuauRuntime::GetGameRuntime();

		m_activeScene = std::make_shared<Scene>();
		

        auto testScript = m_luau->LoadScriptFromFile("assets/scripts/test.luau");


		// m_activeScene->CreateEntity("Test");
		
		auto test2 = m_activeScene->CreateEntity("Script Object");
		test2.AddComponent<LuauScriptComponent>(testScript);

		// m_activeScene->CreateEntity("Test3");

		m_sceneHierarchyPanel.SetContext(m_activeScene);
		m_sceneHierarchyPanel.onSelect = [&](Entity entity) {
			m_selected = entity;
			m_inspector.SetActiveEntity(entity);
			return true;
		};

		console.Initialize(textEditorFont);
		m_activeScene->Initialize();

		return true;
	}

	void Update(float dt) override {
		if (m_activeScene != nullptr) m_activeScene->Update(dt);
		m_luau->Update(dt);
	}

	void DrawIMGUI() override {
		auto vp = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(vp->ID, vp);

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene", "CTRL+N")) {
					m_activeScene = std::make_shared<Scene>();
				}

				if (ImGui::MenuItem("Open Scene...", "CTRL+O")) {
					// OpenScene();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "CTRL+S", false, m_activeScene != nullptr)) {
					// Save();
				}

				if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S")) {
					// SaveAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit", "ALT+F4")) {
					Engine::GetInstance().Quit();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools")) {
				// if (ImGui::MenuItem("Debug", "", showDebugMenu)) {
				// 	showDebugMenu = !showDebugMenu;
				// }

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("About", "", m_aboutWindow)) {
					m_aboutWindow = !m_aboutWindow;
				}

				if (ImGui::MenuItem("Demo Window", "", m_demoWindow)) {
					m_demoWindow = !m_demoWindow;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (m_demoWindow)
			ImGui::ShowDemoWindow(&m_demoWindow);

		if (m_aboutWindow) {
			// ImGui::SetWindowSize(ImVec2(200, 300), ImGuiCond_Always);
			ImGui::Begin("About", &m_aboutWindow, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
			{
				ImGui::TextColored(ImColor(0xFF'b3'80'FF), ANDROMEDA_VERSION_STRING);
				ImGui::Text("Experimental Game Engine by vorlias");
				ImGui::Text("Definitely NOT for production use.");
			}
			ImGui::End();
		}


		m_sceneHierarchyPanel.DrawHierarchyPanel();
		m_inspector.DrawInspector();
		
		
		// ImGui::PushFont(textEditorFont, 20.f);
		// editor.SetLanguage(TextEditor::Language::Luau());
		// editor.SetShowMiniMapEnabled(true);
		// editor.SetPalette(editor.GetDarkPalette());
		// editor.Render("Text Editor");
		
		// ImGui::PopFont();

		console.Draw();
	}


	void Shutdown() override {
		s_editorWindowIcon.Destroy();
	}

private:
	bool m_demoWindow = false;
	bool m_aboutWindow = false;

	SharedRef<Scene> m_activeScene;
    SharedRef<LuauRuntime> m_luau;

	Entity m_selected;

	SceneHierarchyPanel m_sceneHierarchyPanel;
	InspectorPanel m_inspector;
	
	std::filesystem::path m_projectRootPath;
	Console console;

	ImFont* textEditorFont;

	TextEditor editor;
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

	auto current = std::filesystem::current_path();
	return new EditorApplication(current);
}