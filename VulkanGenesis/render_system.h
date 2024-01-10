#pragma once

#include "pipeline.h"
#include "device.h"
#include "model.h"
#include "object.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <memory>
#include <stdexcept>
#include <array>

namespace Lorenz {

	class RenderSystem {
	public:

		RenderSystem(LorenzDevice & device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void renderObjects(VkCommandBuffer commandBuffer, std::vector<Object> &Objects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LorenzDevice &lorenzDevice;

		std::unique_ptr<Pipeline> lorenzPipeline;
		VkPipelineLayout pipelineLayout;
	};
}	// Namespace Lorenz