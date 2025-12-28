#pragma once

#include "foundation/types.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept ArchetypeChunk = 
	   requires
	{
		{ T::get_entity_capacity() } -> std::integral;
		{ T::get_data_count() } -> std::integral;
		{ T::get_data_types() } -> std::meta::reflection_range;
	};
}