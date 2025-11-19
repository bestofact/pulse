#pragma once

#include "app/ecs/module/component/name.h"
#include <iostream>

namespace pulse::ecs::module
{
	void Initialize(const Name& in_name)
	{
		std:: cout << "Init module" << std::endl;
	}
}