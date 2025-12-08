#pragma once

#include <meta>

#include "ecs/v3/entity/entity.h"
#include "ecs/v3/entity/entitycapacity.h"


namespace pulse::ecs::utils
{
	consteval std::meta::info get_entity_template_info()
	{
		return ^^pulse::ecs::Entity;
	}

	consteval std::meta::info get_entity_capacity_type_info()
	{
		return ^^pulse::ecs::EntityCapacity;
	}

	consteval std::meta::info get_entity_capacity_value_type_info()
	{
		return ^^pulse::ecs::EntityCapacity::ValueType;
	}
}