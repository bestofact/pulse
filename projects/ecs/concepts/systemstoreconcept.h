#pragma once

#include "ecs/concepts/systemchunkconcept.h"

#include "foundation/meta.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept SystemStore =
	requires 
	{
		{ T::get_system_chunk_count() } -> std::integral;
		{ T::get_system_chunk_index(std::meta::info()) } -> std::integral;
		{ T::get_nth_system_chunk_type(0) } -> std::same_as<std::meta::info>;
		{ T::get_system_chunk_types() } -> std::meta::reflection_range;

		pulse::meta::is_concept_satisfied_for_all(T::get_system_chunk_types(), ^^pulse::ecs::concepts::SystemChunk);
	};
}