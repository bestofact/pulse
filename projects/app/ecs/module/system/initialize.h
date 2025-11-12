#pragma once

#include "ecs/core/access.h"
#include <iostream>

namespace pulse::ecs::module
{
	void Initialize()
	{
		std:: cout << "Init module" << std::endl;
	}
}