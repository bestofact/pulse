#pragma once
#include "app/modules/entity.h"
#include "app/modules/components.h"
#include "app/modules/systems.h"
#include "ecs/utils/generators.h"

namespace pulse::ecs::modules
{
	consteval std::meta::info get_world_type()
	{
		return pulse::ecs::generators::generate_default_world(^^pulse::ecs::modules);
	}

	using World = [:get_world_type():];
}