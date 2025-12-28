#pragma once

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept ArchetypeStore =
	   requires
	{
		{ T::get_entity_capacity() } -> std::integral;
		{ T::get_archetype_chunk_count() } -> std::integral;
		{ T::get_archetype_chunk_types() } -> std::meta::reflection_range;
	};
}