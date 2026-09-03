#pragma once
#include <Andromeda.h>
#include "Engine/FileDialogs.h"
#include "Engine/File.h"
#include "imgui/imgui.h"
#include <SDL3/SDL_surface.h>

#include "Engine/Events/Event.h"

#include "Widgets/TextEditor.h"
#include "Widgets/IconsLucide.h"

#include "EditorPanels/SceneHeirarchy.h"
#include "EditorPanels/Inspector.h"
#include "EditorPanels/Console.h"
#include "EditorPanels/SceneView.h"

#include "Assets/Assets.h"
#include "Assets/LuauScriptImporter.h"

#include "Settings/UserSettings.h"
#include "Settings/ProjectConfig.h"

namespace andromeda {
	static andromeda::WindowIcon s_editorWindowIcon;

	class EditorApplication : public Application {
	private:
		void SetupAssets();

		void SetupLuau() {
			m_luau = LuauRuntime::GetGameRuntime();
		}

		void SetupIMGUI();

		SharedRef<Scene> NewScene() {
			m_activeScene = std::make_shared<Scene>();
			{
				m_sceneHierarchyPanel.SetContext(m_activeScene);
				m_sceneHierarchyPanel.onSelect = [&](Entity entity) {
					m_selected = entity;
					m_inspector.SetActiveEntity(entity);
					return true;
				};

				m_inspector.SetContext(m_activeScene);
			}
			return m_activeScene;
		}

		void UpdateWindowTitle() {
			std::string title;
			title += projectConfig.ProjectName + " - ";
			title += ANDROMEDA_VERSION_STRING;
			title += " <" + Engine::GetInstance().GetRenderer()->GetAPIString() + ">";

			GetMainWindow()->SetTitle(title.c_str());
		}

		void InitSettings() {
			settings.Load(m_settingsPath);
			projectConfig.Load(m_projectConfigPath);
		}

		void SaveSettings() {
			settings.Save(m_settingsPath);
			projectConfig.Save(m_projectConfigPath);
		}

	public:
		enum EditorFlags {
			EDITOR_DEFAULT = 0,
			NO_EDITOR = 1,
		};

		EditorApplication(const std::filesystem::path& projectPath, EditorFlags flags)
			: m_editorInitFlags(flags)
			, m_projectRootPath(projectPath)
			, m_activeScene(nullptr)
			, m_sceneHierarchyPanel(m_activeScene)
			, m_inspector(m_activeScene) {
			m_projectConfigPath = m_projectRootPath / "AndromedaProject.aproj";
			

			InitSettings();
			SetPersistentDataPath(GetPersistentDataPath() / projectConfig.CompanyName / projectConfig.ProjectName);

			m_settingsPath = GetPersistentDataPath() / "settings.toml";
		}

		bool Initialize() override;

		void Update(float dt) override {
			if (m_showEditor) {
				// Apply any pending rendering changes (like viewport resizing, render commands) before rendering
				m_sceneView.SubmitSceneForRendering();

				m_activeScene->EditorUpdate(dt);
			} else {
				m_activeScene->SubmitSceneForRendering();
			}

			if (m_running) {
				if (m_activeScene != nullptr)
					m_activeScene->Update(dt);
				m_luau->Update(dt);
			}
		}

		void Event(andromeda::Event& e) override {
			EventDispatcher dispatcher(e);

			dispatcher.Dispatch<WindowResizedEvent>(this, &EditorApplication::WindowResized);
			dispatcher.Dispatch<WindowMaximizedEvent>(this, &EditorApplication::WindowMaximized);
			dispatcher.Dispatch<WindowRestoredEvent>(this, &EditorApplication::WindowRestored);
		}

		void DrawIMGUI() override;

		bool WindowResized(WindowResizedEvent& event) {
			settings.windowSize = Vector2(event.width, event.height);
			return true;
		}

		bool WindowMaximized(WindowMaximizedEvent&) {
			settings.maximized = true;
			return true;
		}

		bool WindowRestored(WindowRestoredEvent&) {
			settings.maximized = false;
			return true;
		}

		void Shutdown() override {
			m_sceneView.Shutdown();
			SaveSettings();
			assets.Shutdown();
			s_editorWindowIcon.Destroy();
		}

	private:
		bool m_demoWindow = false;
		bool m_aboutWindow = false;
		bool m_running = true;

		const EditorFlags m_editorInitFlags;

		SharedRef<Scene> m_scene;
		SharedRef<Scene> m_activeScene;

		SharedRef<LuauRuntime> m_luau;

		Entity m_selected;

		SceneHierarchyPanel m_sceneHierarchyPanel;
		InspectorPanel m_inspector;
		Console m_console;

		SceneView m_sceneView;

		std::filesystem::path m_projectRootPath, m_settingsPath, m_projectConfigPath;

		ImFont* textEditorFont;
		ImFont* boldDefaultFont;

		TextEditor editor;
		AssetLibrary assets;

		UserSettings settings;
		ProjectConfig projectConfig;

		bool m_showEditor = true;
	};
} // namespace andromeda