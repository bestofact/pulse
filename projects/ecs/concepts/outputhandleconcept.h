#pragma once

#include "ecs/concepts/outputconcept.h"
#include "foundation/meta.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept OutputHandle = 
	// Require wrapped type to be an output concept.
	   pulse::meta::is_concept_satisfied_for_all(T::get_output_types(), ^^pulse::ecs::concepts::Output)
	&& requires
	{
		// Require get output_type static function.
		{ T::get_output_count() } -> std::integral;
		{ T::get_output_index(std::meta::info()) } -> std::integral;
		{ T::get_nth_output_type(0) } -> std::same_as<std::meta::info>;
		{ T::get_output_types() } -> std::meta::reflection_range;
	};
}