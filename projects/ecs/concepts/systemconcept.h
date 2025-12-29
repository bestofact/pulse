#pragma once

#include "ecs/concepts/archetypemodifierconcept.h"
#include "ecs/concepts/helpers.h"
#include "ecs/concepts/entityconcept.h"
#include "ecs/concepts/outputhandleconcept.h"
#include "ecs/concepts/dataconcept.h"
#include "ecs/concepts/outputconcept.h"

#include "foundation/meta.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
namespace __detail
{
	consteval bool has_valid_return_type_for_system(std::meta::info in_function)
	{
		const auto type = std::meta::return_type_of(in_function);
		return std::meta::is_void_type(type)
			|| pulse::meta::is_concept_satisfied(type, ^^pulse::ecs::concepts::OutputHandle);
	}

	consteval bool has_valid_parameter_types_for_system(std::meta::info in_function)
	{
		const auto parameters = std::meta::parameters_of(in_function);
		for(const auto parameter : parameters)
		{
			const auto type = pulse::meta::decay_all(parameter);
			if(!pulse::meta::is_concept_satisfied(type, ^^pulse::ecs::concepts::Data)
			&& !pulse::meta::is_concept_satisfied(type, ^^pulse::ecs::concepts::ArchetypeModifier))
			{
				return false;
			}
		}
		return true;
	}
}

	template<typename T>
	concept System = 
	   std::meta::is_function_type(^^T)
	&& __detail::has_valid_return_type_for_system(^^T)
	&& __detail::has_valid_parameter_types_for_system(^^T);
}