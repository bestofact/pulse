#pragma once

#include "ecs/entity/entityconcept.h"

#include <meta>

namespace pulse::ecs::utils
{
	consteval bool is_system(std::meta::info in_info)
	{
		return std::meta::is_function(in_info);
	}

	consteval bool is_component(std::meta::info in_info)
	{
		return std::meta::is_complete_type(in_info) && std::meta::is_type(in_info);
	}

	template<pulse::ecs::EntityConcept _entity_type>
	bool is_entity_alive(const _entity_type& in_entity)
	{
		return in_entity.m_bitset.any();
	}
}
