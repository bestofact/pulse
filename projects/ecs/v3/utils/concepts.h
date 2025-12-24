#pragma once

#include <meta>
#include <type_traits>
#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/concepts/entityconcept.h"
#include "ecs/v3/concepts/componentpoolconcept.h"
#include "ecs/v3/concepts/entitycapacityconcept.h"

namespace pulse::ecs::utils
{
	consteval bool is_concept_satisfied(std::meta::info in_typeInfo, std::meta::info in_conceptInfo)
	{
		const auto substituteInfo = std::meta::substitute(in_conceptInfo, {in_typeInfo});
		return substituteInfo != std::meta::info()
			?  std::meta::extract<bool>(substituteInfo)
			:  false;
	}

	consteval std::meta::info get_component_concept_info()
	{
		return ^^pulse::ecs::concepts::Component;
	}

	consteval bool is_component_type(std::meta::info in_typeInfo)
	{
		return std::meta::is_type(in_typeInfo)
			&& is_concept_satisfied(std::meta::decay(in_typeInfo), get_component_concept_info());
	}

	consteval std::meta::info get_entity_concept_info()
	{
		return ^^pulse::ecs::concepts::Entity;
	}

	consteval bool is_entity_type(std::meta::info in_typeInfo)
	{
		return std::meta::is_type(in_typeInfo)
			&& is_concept_satisfied(std::meta::decay(in_typeInfo), get_entity_concept_info());
	}

	consteval std::meta::info get_entity_capacity_concept_info()
	{
		return ^^pulse::ecs::concepts::EntityCapacity;
	}

	consteval bool is_entity_capacity_type(std::meta::info in_typeInfo)
	{
		return std::meta::is_type(in_typeInfo)
			&& is_concept_satisfied(std::meta::decay(in_typeInfo), get_entity_capacity_concept_info());
	}

	consteval bool is_system_function_info(std::meta::info in_info)
	{
		return std::meta::is_function(in_info);
	}
}