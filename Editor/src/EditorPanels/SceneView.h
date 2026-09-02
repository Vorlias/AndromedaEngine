#pragma once
#include "Engine/Graphics/RenderTarget.h"
#include <Andromeda.h>

namespace andromeda {
	class SceneView {
	public:
		SceneView();

		void Initialize();
		void DrawSceneView();

		void SetScene(SharedRef<Scene> scene) {
			m_scene = scene;
		}

		void Shutdown();

		// Prepare the scene for rendering - should be called in _Update_
		void SubmitSceneForRendering();
	private:
		SharedRef<graphics::RenderTexture> m_sceneViewportTexture;
		SharedRef<Scene> m_scene;

		int m_pendingWidth{};
		int m_pendingHeight{};
		bool m_needsResize = true;
	};
} // namespace andromeda