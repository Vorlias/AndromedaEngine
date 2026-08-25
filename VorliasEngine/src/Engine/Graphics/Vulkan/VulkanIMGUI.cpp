#include "Engine/Graphics/Vulkan/VulkanIMGUI.h"
using namespace andromeda;

void VulkanIMGUI::CreateCommandBuffers() {
	auto wd = m_windowData;
}

void VulkanIMGUI::Initialize() {
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	io.DisplaySize.x = width;
	io.DisplaySize.y = height;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForVulkan(m_window);
	
	VkFormat colorFormat = m_vkWindow->swapchainFormat;

	ImGui_ImplVulkan_InitInfo initInfo = {
		.ApiVersion = m_vk->VulkanVersion,
		.Instance = m_vk->GetInstance(),
		.PhysicalDevice = m_vk->GetPhysicalDevice(),
		.Device = m_vk->GetDevice(),
		.QueueFamily = (uint32_t) m_vk->GetGraphicsFamilyIndex(),
		.Queue = m_vk->GetGraphicsQueue(),
		// .DescriptorPool = /* TODO */,
		.DescriptorPoolSize = 8,
		.MinImageCount = m_vkWindow->GetMinImageCount(),
		.ImageCount = m_vkWindow->GetImageCount(),
		.UseDynamicRendering = true,
		.PipelineInfoMain = {
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.Subpass = 0,
			.PipelineRenderingCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
				.pNext = nullptr,
				.viewMask = 0,
				.colorAttachmentCount = 1,
				.pColorAttachmentFormats = &colorFormat,
				.depthAttachmentFormat = m_vkWindow->depthFormat,
				.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
			},
			
			
		},
		.Allocator = nullptr,
	};

	ImGui_ImplVulkan_Init(&initInfo);

	m_commandBuffers.resize(m_vkWindow->GetImageCount());
}

void VulkanIMGUI::UpdateSwapchain() {
	auto wd = m_windowData;

	// Handle swapchain init
	{
		wd->Width = m_vkWindow->GetSwapchainWidth();
		wd->Height = m_vkWindow->GetSwapchainHeight();
		wd->Swapchain = m_vkWindow->GetSwapchain();

		wd->ImageCount = m_vkWindow->GetImageCount();
		wd->SemaphoreCount = m_vkWindow->GetSemaphoreCount();

		wd->SemaphoreCount = wd->ImageCount + 1;
		wd->Frames.resize(wd->ImageCount);
		wd->FrameSemaphores.resize(wd->SemaphoreCount);

		memset(wd->Frames.Data, 0, wd->Frames.size_in_bytes());
		memset(wd->FrameSemaphores.Data, 0, wd->FrameSemaphores.size_in_bytes());

		const auto& images = m_vkWindow->GetSwapchainImages();

		for (uint32_t i = 0; i < wd->ImageCount; i++) {
			wd->Frames[i].Backbuffer = images[i];
		}
	}

	// Create The Image Views
	{
		for (uint32_t i = 0; i < wd->ImageCount; i++) {
			ImGui_ImplVulkanH_Frame* fd = &wd->Frames[i];
			fd->BackbufferView = m_vkWindow->GetImageView(i);
		}
	}
}

void VulkanIMGUI::NewFrame() {
	ImGui::NewFrame();
	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
}

void VulkanIMGUI::Render() {
	ImGui::Render();
	// auto drawData = ImGui::GetDrawData();

	// if (drawData == nullptr)
	// 	return;

	// const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
	// if (!isMinimized) {
	// 	ImGui_ImplVulkan_RenderDrawData(drawData, /*TODO*/ VK_NULL_HANDLE, m_windowData->Pipeline);
	// }
}

void VulkanIMGUI::Shutdown() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
}