#include "application.h"

namespace Lorenz {

	Application::Application()
	{
		float z = 8.f / 3.f;
		startLorenzAttractor(10, 28, z);
		//loadObject();
	}

	Application::~Application()
	{
	}

	void Application::run() {

		std::vector<std::unique_ptr<LorenzBuffer>> uboBuffers(LorenzSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<LorenzBuffer>(
				lorenzDevice,
				sizeof(UniBuffer),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		RenderSystem renderSystem(lorenzDevice, lorenzRenderer.getSwapChainRenderPass());
        Camera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -20.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = Object::createObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lorenzWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(lorenzWindow.getWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lorenzRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 200.f);

			if (auto commandBuffer = lorenzRenderer.beginFrame())
			{
				int frameIndex = lorenzRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// Updating
				UniBuffer ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Rendering
				lorenzRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(frameInfo, objects);
				lorenzRenderer.endSwapChainRenderPass(commandBuffer);
				lorenzRenderer.endFrame();
			}

			//loadObjects();
		}

		vkDeviceWaitIdle(lorenzDevice.device());
	}

	void Application::startLorenzAttractor(float sigma, float rho, float beta)
	{
		float timeStep = 0.01;
		int iterationCount = 200;

		glm::vec3 start{
		0.1f, 
		0.f, 
		0.f
		};

		glm::vec3 nextDot{};

		for (int i = 0; i < iterationCount; i++)
		{
			nextDot.x = start.x + timeStep * sigma * (start.y - start.x);
			nextDot.y = start.y + timeStep * (start.x * ( rho - start.z) - start.y);
			nextDot.z = start.z + timeStep * (start.x * start.y - beta * start.z);

			loadObjects(nextDot);

			start.x = nextDot.x;
			start.y = nextDot.y;
			start.z = nextDot.z;
		}
	}

	void Application::loadObjects(glm::vec3 pos)
	{
        std::shared_ptr<Model> lorenzModel = Model::createModelFromFile(lorenzDevice, "models/Red_Triangular_Pyramid.obj");
        auto cube = Object::createObject();
        cube.model = lorenzModel;
		// Translation is the position of the object. Keep in mind that the Y-axis is reversed.
		//cube.transform.translation = { .0f, .0f, 2.5f };
		cube.transform.translation = {pos.x, -pos.z, pos.y };
        cube.transform.scale = glm::vec3(5.f);
        objects.push_back(std::move(cube));
	}

	// TODO: TEMPORARY, REMOVE WHEN DONE
	void Application::loadObject()
	{
		std::shared_ptr<Model> lorenzModel = Model::createModelFromFile(lorenzDevice, "models/smooth_vase.obj");
		auto cube = Object::createObject();
		cube.model = lorenzModel;
		// Translation is the position of the object. Keep in mind that the Y-axis is reversed.
		//cube.transform.translation = { .0f, .0f, 2.5f };
		cube.transform.translation = { 0.f, 0.f, 2.5f };
		cube.transform.scale = glm::vec3(2.f);
		objects.push_back(std::move(cube));
	}

}	// Namespace Lorenz