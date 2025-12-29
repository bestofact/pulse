#pragma once

#include "app/modules/components/name.h"

#include <print>

namespace pulse::ecs::modules
{
	void PrintNames(const Name& in_name)
	{
		std::println("Module name is = {0}", in_name.m_name);
	}
}