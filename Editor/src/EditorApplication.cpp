#include "EditorApplication.h"
using namespace andromeda;

#ifndef ANDROMEDA_EDITOR_ICO
#	define ANDROMEDA_EDTIOR_ICO
#	include "AndromedaEditorIcon.h"
#endif

andromeda::WindowIcon s_editorWindowIcon(AndromedaIcon, AndromedaIcon_len);

void EditorApplication::SetupAssets() {
	SetDataPath(m_projectRootPath / "assets");

	assets.RegisterImporter<LuauScriptImporter>("luau");
	assets.RegisterImporter<FontImporterTTF>("ttf");
	assets.RegisterImporter<SPIRVShaderImporter>("spv");

	assets.Initialize(m_projectRootPath, GetDataPath(true));
}

bool EditorApplication::Initialize() {
	SetupAssets();
	InitSettings();

	WindowOptions windowOptions("Andromeda Engine", Vector2u(1024, 768), WindowFlags::Resizable);
	windowOptions.windowIcon = s_editorWindowIcon;

	if (settings.windowSize != Vector2(0, 0)) {
		windowOptions.size = settings.windowSize;
	}

	if (settings.windowPosition != Vector2(0, 0)) {
		windowOptions.position = settings.windowPosition;
	}

	if (settings.maximized) {
		windowOptions.window_flags |= WindowFlags::Maximized;
	}

	CreateWindow(windowOptions);
	UpdateWindowTitle();
	SetupIMGUI();
	SetupLuau();

	// m_sceneView.Initialize();

	auto scene = NewScene();
	{
		auto entity = scene->CreateEntity("Script Object");
		auto script = assets.GetAsset<LuauScript>("assets/scripts/test.luau");
		entity.AddComponent<LuauScriptComponent>(script);

		auto childEntity = scene->CreateEntity("Child object");
		childEntity.SetParent(entity);

		auto childEntity2 = scene->CreateEntity("Child object2");
		childEntity2.SetParent(entity);

		auto childEntity3 = scene->CreateEntity();
		childEntity3.SetParent(childEntity2);
	}

	return true;
}

void EditorApplication::SetupIMGUI() {
	InitIMGUI();
	ANDROMEDA_ASSERT(imgui != nullptr);

	auto& io = ImGui::GetIO();
	// io.IniFilename = NULL;

	// std::string imguiIniFile = m_projectRootPath / "imgui.ini";

	// ImGui::LoadIniSettingsFromDisk(imguiIniFile.c_str());

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

	boldDefaultFont = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", 15.0f);
	textEditorFont = io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-Regular.ttf", 15.0f);

	m_console.Initialize(textEditorFont);
}

void EditorApplication::DrawIMGUI() {
	auto vp = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(vp->ID, vp);

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Scene", "CTRL+N")) {
				NewScene();
			}

			// if (ImGui::MenuItem("Open Scene...", "CTRL+O")) {
			// }

			// ImGui::Separator();

			// if (ImGui::MenuItem("Save", "CTRL+S", false, m_activeScene != nullptr)) {
			// }

			// if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S")) {
			// }

			ImGui::Separator();

			if (ImGui::MenuItem("Exit", "ALT+F4")) {
				Engine::GetInstance().Quit();
			}
			ImGui::EndMenu();
		}

		// if (ImGui::BeginMenu("Tools")) {
		// 	ImGui::EndMenu();
		// }

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

	// m_sceneView.DrawSceneView();

	// ImGui::PushFont(textEditorFont, 20.f);
	// {
	// 	editor.SetLanguage(TextEditor::Language::Luau());
	// 	editor.SetShowMiniMapEnabled(true);
	// 	editor.SetPalette(editor.GetDarkPalette());
	// 	editor.Render("Text Editor");
	// }
	// ImGui::PopFont();

	m_console.Draw();
}