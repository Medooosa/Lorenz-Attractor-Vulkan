#pragma once

#include "window.h"
#include "device.h"
#include "model.h"
#include "object.h"
#include "render_engine.h"
#include "render_system.h"
#include "point_light.h"
#include "camera.h"
#include "user_input_controller.h"
#include "descriptor.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <memory>
#include <stdexcept>
#include <array>
#include <chrono>
#include <numeric>

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
		void startLorenzAttractor(float sigma, float rho, float beta);

	private:
		void loadObjects(glm::vec3 pos);
		void loadObject();


		LorenzWindow lorenzWindow{ WIDTH, HEIGHT, "Hello World!" };
		LorenzDevice lorenzDevice{ lorenzWindow };
		Renderer lorenzRenderer{ lorenzWindow, lorenzDevice };

		std::unique_ptr<LorenzDescriptorPool> descriptorPool{};
		Object::Map objects;

		const float MAX_FRAME_TIME = 0.1f;
	};
}	// Namespace Lorenz