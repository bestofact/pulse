#pragma once

#include <iostream>

namespace pulse::ecs::module
{
	void Initialize()
	{
		std:: cout << "Init module" << std::endl;
	}

	void Initialize2()
	{
		std:: cout << "Init module 2" << std::endl;
	}

	bool test()
	{
		std:: cout << "test" << std::endl;
		return false;
	}
}