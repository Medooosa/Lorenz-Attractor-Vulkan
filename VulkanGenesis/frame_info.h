#pragma once

#include "camera.h"
#include "object.h"

#include <vulkan/vulkan.h>

namespace Lorenz {
	// Frameinfo struct to simplify the parameters. No need to change every function call every time you change a functions parameters.
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
		VkDescriptorSet descriptorSet;
		Object::Map& sceneObjects;
	};

#define MAX_LIGHTS 10
	struct PointLight {
		glm::vec4 position{};
		glm::vec4 color{};
	};
	// Always check the alignment rules when changing buffer objects
	struct UniBuffer
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;

	};

}  // namespace Lorenz