#pragma once
#include "Engine/Graphics/RenderTarget.h"
#include <Andromeda.h>

namespace andromeda {
	class SceneView {
	public:
		SceneView();

		void Initialize();
		void DrawSceneView();

		void Render();

		void SetScene(SharedRef<Scene> scene) {
			m_scene = scene;
		}
	private:
		SharedRef<graphics::RenderTexture> m_sceneViewportTexture;
		SharedRef<Scene> m_scene;
	};
} // namespace andromeda