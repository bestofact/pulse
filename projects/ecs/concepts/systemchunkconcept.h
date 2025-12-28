#pragma once

#include "ecs/concepts/systemhandleconcept.h"

#include "foundation/meta.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept SystemChunk = 
	requires 
	{
		{ T::get_system_handle_count() } -> std::integral;
		{ T::get_system_handle_index(std::meta::info()) } -> std::integral;
		{ T::get_nth_system_handle_type(0) } -> std::same_as<std::meta::info>;
		{ T::get_system_handle_types() } -> std::meta::reflection_range;

		pulse::meta::is_concept_satisfied_for_all(T::get_system_handle_types(), ^^pulse::ecs::concepts::SystemHandle);
	};
}