#pragma once

#include "window.h"
#include "pipeline.h"
#include "device.h"
#include "swap_chain.h"
#include "model.h"

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
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		LorenzWindow lorenzWindow{ WIDTH, HEIGHT, "Hello World!" };
		LorenzDevice lorenzDevice{ lorenzWindow };
		std::unique_ptr<LorenzSwapChain> lorenzSwapChain;
		std::unique_ptr<Pipeline> lorenzPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<Model> lorenzModel;
	};
}	// Namespace Lorenz