#include "application.h"

namespace Lorenz {

	Application::Application()
	{
		loadObjects();
	}

	Application::~Application()
	{
	}

	void Application::run() {
		RenderSystem renderSystem(lorenzDevice, lorenzRenderer.getSwapChainRenderPass());
		while (!lorenzWindow.shouldClose()) {
			glfwPollEvents();
			if (auto commandBuffer = lorenzRenderer.beginFrame())
			{
				lorenzRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(commandBuffer, objects);
				lorenzRenderer.endSwapChainRenderPass(commandBuffer);
				lorenzRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lorenzDevice.device());
	}

	void Application::loadObjects()
	{
		std::vector<Model::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

		auto lorenzModel = std::make_shared<Model>(lorenzDevice, vertices);
		
		auto triangle = Object::createObject();
		triangle.model = lorenzModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();

		objects.push_back(std::move(triangle));
	}

}	// Namespace Lorenz