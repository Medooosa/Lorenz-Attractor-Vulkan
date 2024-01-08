#pragma once

#include "window.h"
#include "pipeline.h"
#include "device.h"
#include "swap_chain.h"

// std
#include <memory>
#include <stdexcept>
#include <array>

namespace Lorenz {

	class Application {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application &) = delete;
		Application &operator=(const Application &) = delete;

		void run();

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		LorenzWindow lorenzWindow{ WIDTH, HEIGHT, "Hello World!" };
		LorenzDevice lorenzDevice{ lorenzWindow };
		LorenzSwapChain lorenzSwapChain{lorenzDevice, lorenzWindow.getExtent()};
		std::unique_ptr<Pipeline> lorenzPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}	// Namespace Lorenz