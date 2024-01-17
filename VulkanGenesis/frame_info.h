#pragma once

#include "camera.h"

#include <vulkan/vulkan.h>

namespace Lorenz {
	// Frameinfo struct to simplify the parameters. No need to change every function call every time you change a functions parameters.
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
	};
}  // namespace Lorenz