#include "Engine/Graphics/Vulkan/VulkanIMGUI.h"
#include "Engine/IMGUIStyle.h"
using namespace andromeda;

void VulkanIMGUI::CreateCommandBuffers() {
	auto wd = m_windowData;
}

void VulkanIMGUI::Initialize() {
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	io.DisplaySize.x = width;
	io.DisplaySize.y = height;

	auto& style = ImGui::GetStyle();
	// style.FontScaleMain = 1.0f;
	ImGui::StyleColorsDark();
	ImGui::GetStyle() = GetImguiStyle();

	ImGui_ImplSDL3_InitForVulkan(m_window);

	VkFormat colorFormat = m_vkWindow->swapchainFormat;
	ImGui_ImplVulkan_InitInfo initInfo = {
		.ApiVersion = m_vk->VulkanVersion,
		.Instance = m_vk->GetInstance(),
		.PhysicalDevice = m_vk->GetPhysicalDevice(),
		.Device = m_vk->GetDevice(),
		.QueueFamily = (uint32_t) m_vk->GetGraphicsFamilyIndex(),
		.Queue = m_vk->GetGraphicsQueue(),
		// .DescriptorPool
		.DescriptorPoolSize = 8,
		.MinImageCount = m_vkWindow->GetMinImageCount(),
		.ImageCount = m_vkWindow->GetImageCount(),
		.PipelineInfoMain = {
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.PipelineRenderingCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
				.pNext = nullptr,
				.viewMask = 0,
				.colorAttachmentCount = 1,
				.pColorAttachmentFormats = &colorFormat,
				.depthAttachmentFormat = m_vkWindow->depthFormat,
				.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
			},
		},
		.UseDynamicRendering = true,
	};

	ANDROMEDA_ASSERT(ImGui_ImplVulkan_Init(&initInfo));

	// auto& frameRes = m_vkWindow->GetFrameResources();
	// m_commandBuffers.resize(frameRes.size());
	// frameRes[0].commandBuffer

	// for (auto& frame : m_frames) {
	// 	frame.Backbuffer
	// }
	m_active = true;
}

void VulkanIMGUI::Resize(int width, int height) {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = width;
	io.DisplaySize.y = height;
}

void VulkanIMGUI::UpdateSwapchain() {

}

bool VulkanIMGUI::ProcessEvent(SDL_Event& e) {
	return (ImGui_ImplSDL3_ProcessEvent(&e));
}

void VulkanIMGUI::NewFrame() {
	
	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();
}

void VulkanIMGUI::Render() {
	ImGui::Render();
	auto drawData = ImGui::GetDrawData();

	if (drawData == nullptr)
		return;

	const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
	if (isMinimized) return;

	ImGui_ImplVulkan_RenderDrawData(drawData, m_vkWindow->GetCommandBuffer());
}

void VulkanIMGUI::Shutdown() {
	vkDeviceWaitIdle(m_vk->GetDevice());
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	m_active = false;
}