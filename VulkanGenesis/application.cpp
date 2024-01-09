#include "application.h"

namespace Lorenz {

	Application::Application()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(lorenzDevice.device(), pipelineLayout, nullptr);
	}

	void Application::run() {
		while (!lorenzWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(lorenzDevice.device());
	}

	void Application::loadModels()
	{
		std::vector<Model::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

		lorenzModel = std::make_unique<Model>(lorenzDevice, vertices);
	}

	void Application::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(lorenzDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void Application::createPipeline()
	{
		assert(lorenzSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = lorenzSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lorenzPipeline = std::make_unique<Pipeline>(lorenzDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void Application::recreateSwapChain()
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
			lorenzSwapChain = std::make_unique<LorenzSwapChain>(lorenzDevice, extent, std::move(lorenzSwapChain));
			if (lorenzSwapChain->imageCount() != commandBuffers.size())
			{
				freeCommandBuffers(); 
				createCommandBuffers();
			}
		}
		createPipeline();
	}

	void Application::createCommandBuffers()
	{
		commandBuffers.resize(lorenzSwapChain->imageCount());

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

	void Application::freeCommandBuffers()
	{
		vkFreeCommandBuffers(lorenzDevice.device(), lorenzDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void Application::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lorenzSwapChain->getRenderPass();
		renderPassInfo.framebuffer = lorenzSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = lorenzSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lorenzSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lorenzSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, lorenzSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		lorenzPipeline->bind(commandBuffers[imageIndex]);
		lorenzModel->bind(commandBuffers[imageIndex]);
		lorenzModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void Application::drawFrame()
	{
		uint32_t imageIndex;
		auto result = lorenzSwapChain->acquireNextImage(&imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(); 
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swapchain image");
		}

		recordCommandBuffer(imageIndex);
		result = lorenzSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lorenzWindow.wasWindowResized())
		{
			lorenzWindow.resetWindowResizedFlag(); 
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image");
		}
	}
}	// Namespace Lorenz