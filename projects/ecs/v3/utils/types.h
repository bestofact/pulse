#pragma once

#include <meta>

#include "ecs/v3/entity/entity.h"


namespace pulse::ecs::utils
{
	consteval std::meta::info get_entity_template_info()
	{
		return ^^pulse::ecs::Entity;
	}
}