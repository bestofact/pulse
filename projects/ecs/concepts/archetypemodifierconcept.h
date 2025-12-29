#pragma once


#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept ArchetypeModifier = 
	requires (T t)
	{
		{ T::get_entity_type() } -> std::same_as<std::meta::info>;
		{ T::get_store_type() } -> std::same_as<std::meta::info>;
		{ t.set() };
		{ t.reset() };
	};
}