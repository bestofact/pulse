#pragma once

#include "ecs/concepts/entityconcept.h"
#include "foundation/types.h"

#include <concepts>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept EntityStore =
	   requires
	{
		typename T::EntityType;
		{ T::get_entity_type() } -> std::same_as<std::meta::info>;
		{ T::get_capacity() } -> std::same_as<u64>;
	}
	&& requires (T t) 
	{
		{ t.new_entity() } -> pulse::ecs::concepts::Entity;
		{ t.free_entity(typename T::EntityType()) };
	};
}