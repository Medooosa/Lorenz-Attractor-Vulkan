#pragma once

#include "device.h"
#include "model.h"

// std
#include <string>
#include <vector>

namespace Lorenz {

	struct PipelineConfigInfo{
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		//VkViewport viewport;
		//VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnabled;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline {
	public:
		Pipeline(LorenzDevice& device, const std::string& vertFilePath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
		~Pipeline();


		// Delete operator to avoid copying pointers
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline() = default;
		void bind(VkCommandBuffer commandBuffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		LorenzDevice& lorenzDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

	};
}	// Namespace Lorenz