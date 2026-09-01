#include "SceneView.h"
#include "imgui/imgui.h"

#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"

andromeda::SceneView::SceneView() {}

void andromeda::SceneView::Initialize() {
	m_sceneViewportTexture = andromeda::graphics::RenderTexture::Create(andromeda::graphics::RenderTextureType::IMGUITexture);

    // auto& engine = Engine::GetInstance();
    // auto ctx = static_cast<graphics::VulkanWindowContext*>(engine.GetMainWindow()->GetGraphicsContext());
    // ctx->SetRenderTarget(m_sceneViewportTexture);
}

void andromeda::SceneView::DrawSceneView() {
	ImGui::Begin("Scene");
	// {
	// 	ImVec2 avail = ImGui::GetContentRegionAvail();
	// 	int width = static_cast<int>(avail.x);
	// 	int height = static_cast<int>(avail.y);

	// 	if (width > 0 && height > 0 && m_sceneViewportTexture != nullptr) {
	// 		if (m_sceneViewportTexture->GetWidth() != width || m_sceneViewportTexture->GetHeight() != height) {
	// 			m_sceneViewportTexture->Resize(width, height);
	// 		}
	// 		ImGui::Image(m_sceneViewportTexture->GetImGuiTexture(), avail);
	// 	} else {
    //         ImGui::Text("No scene");
    //     }
	// }
	ImGui::End();
}

void andromeda::SceneView::Render() {
    if (m_sceneViewportTexture == nullptr) return;
}