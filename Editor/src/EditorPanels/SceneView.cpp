#include "SceneView.h"
#include "imgui/imgui.h"

#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/RenderCommand.h"

andromeda::SceneView::SceneView()
	: m_pendingWidth(0), m_pendingHeight(0), m_needsResize(false) {}

void andromeda::SceneView::Initialize() {
	m_sceneViewportTexture = andromeda::graphics::RenderTexture::Create(andromeda::graphics::RenderTextureType::IMGUITexture);

    auto& engine = Engine::GetInstance();
    auto ctx = static_cast<graphics::VulkanWindowContext*>(engine.GetMainWindow()->GetGraphicsContext());
    ctx->SetRenderTarget(m_sceneViewportTexture);
	m_sceneViewportTexture->SetClearColor(Color(.3f, .5f, .8f));
}

void andromeda::SceneView::DrawSceneView() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::Button("Play")) {

		}

		ImGui::EndMenuBar();
	}

	{
		ImVec2 avail = ImGui::GetContentRegionAvail();
		int width = static_cast<int>(avail.x);
		int height = static_cast<int>(avail.y);

		if (width > 0 && height > 0 && m_sceneViewportTexture != nullptr) {
			if (m_sceneViewportTexture->GetWidth() != width || m_sceneViewportTexture->GetHeight() != height) {
				m_pendingWidth = width;
				m_pendingHeight = height;
				m_needsResize = true;
			}
			ImGui::Image(m_sceneViewportTexture->GetImGuiTexture(), avail);
		} else {
            ImGui::Text("No scene %dx%d %d", width, height, m_sceneViewportTexture != nullptr);
        }
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void andromeda::SceneView::SubmitSceneForRendering() {
	if (m_sceneViewportTexture == nullptr) return;
	if (m_needsResize) {
		m_sceneViewportTexture->Resize(m_pendingWidth, m_pendingHeight);
		m_needsResize = false;
	}

	m_scene->SubmitSceneForRendering();
}

void andromeda::SceneView::Shutdown() {
	if (m_sceneViewportTexture)
		m_sceneViewportTexture->Destroy();
}