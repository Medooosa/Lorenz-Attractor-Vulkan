#include "application.h"

#include <stdexcept>
#include <iostream>
#include <cstdlib>

// Parts of this project were adapted from https://vulkan-tutorial.com and Brendan Galea's vulkan game engine tutorial.

int main() 
{
	Lorenz::Application app{};

	try 
	{
		app.run();
	}

	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}