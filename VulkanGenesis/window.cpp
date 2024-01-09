#include "window.h"
#include <stdexcept>

namespace Lorenz 
{
	LorenzWindow::LorenzWindow(int width, int height, std::string name) : width{ width }, height{ height }, window_name{ name } {
		initWindow();
	}

	LorenzWindow::~LorenzWindow(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LorenzWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void LorenzWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void LorenzWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto lorenzWindow = reinterpret_cast<LorenzWindow *>(glfwGetWindowUserPointer(window));
		lorenzWindow->framebufferResized = true;
		lorenzWindow->width = width;
		lorenzWindow->height = height;
	}

}