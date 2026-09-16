#pragma once
#include "VulkanShader.h"
#include "VulkanInstance.h"

namespace andromeda::graphics {
	enum class PrimitiveTopology {
		TriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	enum class VulkanPolygonMode {
		Fill = VK_POLYGON_MODE_FILL,
		Line = VK_POLYGON_MODE_LINE,
		Point = VK_POLYGON_MODE_POINT,
	};

	class VulkanGraphicsPipeline {
	public:
		VulkanGraphicsPipeline(VulkanContext* vk, VkFormat colorFormat, VkFormat depthFormat, VulkanShader* shader)
			: m_vk(vk), m_shader(shader), m_colorFormat(colorFormat), m_depthFormat(depthFormat) {}

		constexpr void SetPolygonMode(VulkanPolygonMode polygonMode) { m_polygonMode = polygonMode; }

		bool Create();
		void Destroy();

		constexpr VkPipeline GetPipeline() const { return m_graphicsPipeline; }
		constexpr VkPipelineLayout GetPipelineLayout() const { return m_graphicsPipelineLayout; }
		constexpr VulkanShader* GetShader() const { return m_shader; }

        ~VulkanGraphicsPipeline();
	private:
		VulkanContext* m_vk = nullptr;
		VulkanShader* m_shader = nullptr;

		VkFormat m_colorFormat;
		VkFormat m_depthFormat;

		VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_graphicsPipelineLayout = VK_NULL_HANDLE;

		PrimitiveTopology m_primitiveType = PrimitiveTopology::TriangleList;
		VulkanPolygonMode m_polygonMode = VulkanPolygonMode::Fill;
	};
} // namespace andromeda::graphics