#pragma once
#include "app/modules/entity.h"
#include "app/modules/components.h"

#include "ecs/utils/generators.h"

namespace pulse::ecs::modules
{
	consteval std::meta::info get_modifier_type()
	{
		return pulse::ecs::generators::generate_default_component_modifier(^^pulse::ecs::modules);
	}

	using Modifier = [:get_modifier_type():];
}
