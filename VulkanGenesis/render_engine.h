#pragma once

#include "window.h"
#include "device.h"
#include "swap_chain.h"
#include "model.h"

// std
#include <memory>
#include <stdexcept>
#include <array>
#include <cassert>

namespace Lorenz {

	class Renderer {
	public:

		Renderer(LorenzWindow &window, LorenzDevice &device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return lorenzSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCommandBuffer() const {
			assert(isFrameStarted && "Cant get command buffer if frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() {
			assert(isFrameStarted && "Cannot get frame index when frame isnt in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		LorenzWindow &lorenzWindow;
		LorenzDevice &lorenzDevice;
		std::unique_ptr<LorenzSwapChain> lorenzSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex{ 0 };
		int currentFrameIndex;
		bool isFrameStarted{ false };
	};
}	// Namespace Lorenz