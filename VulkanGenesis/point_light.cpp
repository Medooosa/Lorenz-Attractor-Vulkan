#include "point_light.h"

namespace Lorenz {

	struct PointLightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		float radius{};
	};

	PointLightSys::PointLightSys(LorenzDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout) : lorenzDevice{ device }
	{
		createPipelineLayout(descriptorSetLayout);
		createPipeline(renderPass);
	}

	PointLightSys::~PointLightSys()
	{
		vkDestroyPipelineLayout(lorenzDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightSys::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(lorenzDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void PointLightSys::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lorenzPipeline = std::make_unique<Pipeline>(lorenzDevice, "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", pipelineConfig);
	}

	void PointLightSys::update(FrameInfo& frameInfo, UniBuffer& ubo)
	{
		int lightIndex = 0;
		for (auto& keyValue: frameInfo.sceneObjects)
		{
			auto& obj = keyValue.second;
			if (obj.pointLight == nullptr) continue;

			assert(lightIndex < MAX_LIGHTS && "Too many pointlights");
			
			ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
			ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			
			lightIndex += 1;
		}
		ubo.numLights = lightIndex;
	}


	void PointLightSys::render(FrameInfo& frameInfo)
	{
		lorenzPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.descriptorSet,
			0,
			nullptr);

		for (auto& keyValue : frameInfo.sceneObjects)
		{
			auto& obj = keyValue.second;
			if (obj.pointLight == nullptr) continue;
			
			PointLightPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.f);
			push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			push.radius = obj.transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);
			
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);

			
		}
		
	}
}	// Namespace Lorenz