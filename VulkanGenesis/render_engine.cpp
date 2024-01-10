#include "render_engine.h"

namespace Lorenz {

	Renderer::Renderer(LorenzWindow &window, LorenzDevice &device) : lorenzWindow{window}, lorenzDevice{device}
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	Renderer::~Renderer()
	{
		freeCommandBuffers();
	}

	void Renderer::recreateSwapChain()
	{
		auto extent = lorenzWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = lorenzWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(lorenzDevice.device());

		if (lorenzSwapChain == nullptr)
		{
			lorenzSwapChain = std::make_unique<LorenzSwapChain>(lorenzDevice, extent);
		}
		else
		{
			std::shared_ptr<LorenzSwapChain> oldSwapChain = std::move(lorenzSwapChain);
			lorenzSwapChain = std::make_unique<LorenzSwapChain>(lorenzDevice, extent, oldSwapChain);
			if (!oldSwapChain->compareSwapFormats(*lorenzSwapChain.get()))
			{
				throw std::runtime_error("Swapchain image/depth format has changed");
			}
		}
	}

	void Renderer::createCommandBuffers()
	{
		commandBuffers.resize(LorenzSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lorenzDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(lorenzDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(lorenzDevice.device(), lorenzDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer Renderer::beginFrame()
	{
		assert(!isFrameStarted && "Cant call beginFrame when a frame is in progress");
		
		auto result = lorenzSwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swapchain image");
		}
		isFrameStarted = true;

		auto commandBuffer = getCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}
		return commandBuffer;
	}

	void Renderer::endFrame()
	{
		assert(isFrameStarted && "Cant call endframe when frame is not in progress");
		auto commandBuffer = getCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}

		auto result = lorenzSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lorenzWindow.wasWindowResized())
		{
			lorenzWindow.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % LorenzSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cant call beginSwapChainRenderPass when frame is not in progress");
		assert(getCommandBuffer() && "Cant begin renderpass on command buffer from a different frame");
		
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lorenzSwapChain->getRenderPass();
		renderPassInfo.framebuffer = lorenzSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = lorenzSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lorenzSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lorenzSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, lorenzSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	}

	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cant call endSwapChainRenderPass when frame is not in progress");
		assert(getCommandBuffer() && "Cant end renderpass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}

}	// Namespace Lorenz