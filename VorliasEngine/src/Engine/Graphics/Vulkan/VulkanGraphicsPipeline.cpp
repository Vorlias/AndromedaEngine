#include "Engine/Graphics/Vulkan/VulkanGraphicsPipeline.h"
#include "VulkanUtils.h"

bool andromeda::graphics::VulkanGraphicsPipeline::Create() {
	if (m_shader == nullptr) {
		andromeda::error("Pipeline shaders aren't defined");
		return false;
	}

	andromeda::trace("Creating graphics pipeline");
	auto device = m_vk->GetDevice();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pushConstantRangeCount = 0,
	};

	VK_CHECK_ELSE_RETURN(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_graphicsPipelineLayout), false);
	auto mods = m_shader->GetShaderModules();

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.resize(mods.size());
	for (int i = 0; i < mods.size(); i++) {
		auto& mod = mods[i];
		shaderStages[i] = mod.GetShaderStage();
	}

	// vertex pulling, don't define vertex input details
	VkPipelineVertexInputStateCreateInfo vertInputInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	};

	// input assembly, we'll be drawing triangle lists
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = (VkPrimitiveTopology) m_primitiveType,
	};

	// depth/stencil configuration
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.stencilTestEnable = VK_FALSE,
	};

	// dynamic rendering allows to set this up.. dynamically
	// we'll still need this struct though
	VkPipelineViewportStateCreateInfo viewportInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = nullptr, // will change viewport dynamically
		.scissorCount = 1,
		.pScissors = nullptr, // will be dynamic
	};

	// rasterizer settings (which parts of screen need to be rasterized - culling/shading)
	VkPipelineRasterizationStateCreateInfo rasterInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = (VkPolygonMode) m_polygonMode, // entire area filled, also wire can be done here too
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth = 1.0f, // in wireframe mode this changes things
	};

	// no multisampling (anti-aliasing)
	VkPipelineMultisampleStateCreateInfo multiSampleInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT, // 1 bit = no AA
	};

	// Alpha-blending (disabled for now) still need
	// attachment info and write mask
	VkPipelineColorBlendAttachmentState attachState{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo blendInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &attachState,
	};

	// enable dynamic states
	std::vector<VkDynamicState> dynamicState{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	VkPipelineDynamicStateCreateInfo dynamicStateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicState.size()),
		.pDynamicStates = dynamicState.data(),
	};

	// structure required for dynamic rendering (Reduces boilerplate)
	VkPipelineRenderingCreateInfo renderInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &m_colorFormat,
		.depthAttachmentFormat = m_depthFormat,
	};

	// Create the graphics pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &renderInfo, // passing dynamic rendering structure to pNext to enable it
		.stageCount = static_cast<uint32_t>(shaderStages.size()),
		.pStages = shaderStages.data(), // shader modules we're using
		.pVertexInputState = &vertInputInfo,
		.pInputAssemblyState = &inputAssemblyInfo,
		.pViewportState = &viewportInfo,
		.pRasterizationState = &rasterInfo,
		.pMultisampleState = &multiSampleInfo,
		.pDepthStencilState = &depthStencilInfo,
		.pColorBlendState = &blendInfo,
		.pDynamicState = &dynamicStateInfo,
		.layout = m_graphicsPipelineLayout,
		.renderPass = VK_NULL_HANDLE, // we're using dynamic rendering so this will be created automatically
	};

	if (vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		andromeda::error("Failed to create pipeline");
		return false;
	}

	andromeda::trace("Created graphics pipeline");
	return true;
}

void andromeda::graphics::VulkanGraphicsPipeline::Destroy() {
	if (m_graphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(m_vk->GetDevice(), m_graphicsPipeline, nullptr);
	}

	if (m_graphicsPipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(m_vk->GetDevice(), m_graphicsPipelineLayout, nullptr);
	}

	m_shader = nullptr;
	m_vk = nullptr;

	andromeda::trace("Destroyed graphics pipeline");
}

andromeda::graphics::VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
	Destroy();
}