#include "application.h"

#include <stdexcept>
#include <iostream>
#include <cstdlib>

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