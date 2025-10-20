#pragma once

#include <iostream>

namespace pulse::ecs::module
{
	inline void Initialize()
	{
		std::cout << "Initializing module" << std::endl;
	}
}