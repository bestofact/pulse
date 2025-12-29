#pragma once

#include "ecs/concepts/systemconcept.h"

#include "foundation/meta.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept SystemHandle = 
	requires
	{
		{ T::get_system() } -> std::same_as<std::meta::info>;
		{ T::get_system_type() } -> std::same_as<std::meta::info>;
		{ T::get_return_type() } -> std::same_as<std::meta::info>;
		{ T::get_system_pointer_type() } -> std::same_as<std::meta::info>;
		{ T::get_system_instance() } -> std::same_as<typename [:T::get_system_pointer_type():]>;
		{ T::get_constant_required_component_types() } -> std::meta::reflection_range;
		{ T::get_mutable_required_component_types() } -> std::meta::reflection_range;
		{ T::get_produced_output_types() } -> std::meta::reflection_range;
		{ T::get_consumed_output_types() } -> std::meta::reflection_range;
		{ T::get_modifier_type() } -> std::same_as<std::meta::info>;

	  	pulse::meta::is_concept_satisfied(T::get_system_type(), ^^pulse::ecs::concepts::System);
	};
}