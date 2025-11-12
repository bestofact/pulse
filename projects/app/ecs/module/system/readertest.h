#pragma once

#include "ecs/access/reader.h"
#include "app/ecs/module/component/name.h"
#include <iostream>

namespace pulse::ecs::module
{
	void ReadTest(
		const Name& in_name
		)
	{
		std::cout<< "Read Test" << in_name.m_name << std::endl;
	}
}