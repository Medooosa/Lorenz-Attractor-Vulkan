#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Lorenz 
{
	class LorenzWindow 
	{

	public:
		LorenzWindow(int width, int height, std::string name);
		~LorenzWindow();

		// Remove copy constructor, operator. Do not want to copy, we might be left with a dangling pointer. We already have a pointer to the GLFWwindow
		LorenzWindow(const LorenzWindow &) = delete;
		LorenzWindow& operator=(const LorenzWindow &) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return {static_cast<uint32_t>(width, height)}; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		void initWindow();

		const int width;
		const int height;

		std::string window_name;
		GLFWwindow* window;
	};
} // namespace Lorenz