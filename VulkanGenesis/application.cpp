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
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = lorenzRenderer.beginFrame())
			{
				lorenzRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(commandBuffer, objects, camera);
				lorenzRenderer.endSwapChainRenderPass(commandBuffer);
				lorenzRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lorenzDevice.device());
	}

	glm::vec3 Application::lorenz(float sigma, float rho, float beta)
	{

	}

	void Application::loadObjects()
	{
        std::shared_ptr<Model> lorenzModel = Model::createModelFromFile(lorenzDevice, "models/smooth_vase.obj");
        auto cube = Object::createObject();
        cube.model = lorenzModel;
		// Translation is the position of the object. Keep in mind that the Y-axis is reversed.
        cube.transform.translation = { .0f, 1.0f, 2.5f };
        cube.transform.scale = glm::vec3(3.f);
        objects.push_back(std::move(cube));
	}

}	// Namespace Lorenz