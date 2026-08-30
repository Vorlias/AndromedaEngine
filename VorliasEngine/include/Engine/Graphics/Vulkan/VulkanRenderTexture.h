#pragma once
#include "VulkanBase.h"
#include "Engine/Graphics/RenderTarget.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"

#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"

#include "Engine/Graphics/Vulkan/VulkanIMGUI.h"

namespace andromeda::graphics {
	class VulkanRenderTexture : public RenderTarget {
	public:
		enum RenderTextureFlags {
			RENDER_TEXTURE_NONE = 0,
			/** Enable rendering to IMGUI */
			RENDER_TEXTURE_IMGUI = 1,
		};

		VulkanRenderTexture() = default;
		VulkanRenderTexture(RenderTextureFlags textureType) : m_textureType(textureType) {}
		~VulkanRenderTexture();

		VulkanRenderTexture(const VulkanRenderTexture&) = delete;
		VulkanRenderTexture& operator=(const VulkanRenderTexture&) = delete;

		bool Create(
			VulkanContext* vk,
			VulkanWindowContext* vkw /* TODO: Make this not required*/,
			int width,
			int height,
			VkFormat format = VK_FORMAT_R8G8B8A8_SRGB
		);
		void Destroy() override;
		void Resize(int width, int height) override;

		void BeginRender(const FrameResources& res);
		void EndRender(const FrameResources& res);

		VkImage GetImage() const {
			return m_image;
		}

		VkImageView GetImageView() const {
			return m_imageView;
		}

		VkSampler GetSampler() const {
			return m_sampler;
		}

		int GetWidth() const override {
			return m_width;
		}
		int GetHeight() const override {
			return m_height;
		}

		VkFormat GetFormat() const {
			return m_format;
		}

		ImTextureID GetImGuiTexture() const {
			ANDROMEDA_ASSERTM(m_imguiDescriptor != nullptr, "To use IMGUI textures the RENDER_TEXTURE_IMGUI flag must be passed");
			return reinterpret_cast<ImTextureID>(m_imguiDescriptor);
		}

		VkDescriptorSet GetImGuiDescriptor() const {
			return m_imguiDescriptor;
		}

		// Create the IMGUI descriptor - must be called _after_ IMGUI is created
		void CreateImGuiDescriptor(VulkanIMGUI* imgui) {
			ANDROMEDA_ASSERTM(imgui->IsActive(), "IMGUI should be active when creating this descriptor");
			m_imguiDescriptor = ImGui_ImplVulkan_AddTexture(m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			assert(m_imguiDescriptor != VK_NULL_HANDLE);
		}

		// Destroy the IMGUI descriptor - must be called _before_ IMGUI is shut down
		void DestroyImGuiDescriptor(VulkanIMGUI* imgui) {
			ANDROMEDA_ASSERTM(imgui->IsActive(), "IMGUI should still be active when disposing this descriptor");
			if (m_imguiDescriptor != VK_NULL_HANDLE) {
				ImGui_ImplVulkan_RemoveTexture(m_imguiDescriptor);

				m_imguiDescriptor = VK_NULL_HANDLE;
			}
		}

		void TransitionToColorAttachment(VkCommandBuffer commandBuffer);
		void TransitionToShaderRead(VkCommandBuffer commandBuffer);
	private:
		bool CreateImage();
		bool CreateImageView();
		bool CreateSampler();
		void TransitionImage(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

	protected:
		VkImage m_image = VK_NULL_HANDLE;
		VkImageView m_imageView = VK_NULL_HANDLE;
		VkSampler m_sampler = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;

		VkFormat m_format = VK_FORMAT_B8G8R8A8_SRGB;
		VkDescriptorSet m_imguiDescriptor = VK_NULL_HANDLE;
		VkImageLayout m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	private:
		VulkanContext* m_ctx = VK_NULL_HANDLE;
		VulkanWindowContext* m_wctx = VK_NULL_HANDLE;

		VkImageMemoryBarrier m_imageMemoryBarrier;

		uint32_t m_width = 0;
		uint32_t m_height = 0;

		RenderTextureFlags m_textureType = RENDER_TEXTURE_NONE;
	};
} // namespace andromeda::graphics