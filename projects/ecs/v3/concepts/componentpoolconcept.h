#pragma once

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept ComponentPool =
	requires
	{
	    { T::get_component_pool_template_info() } -> std::same_as<std::meta::info>;
		{ T::get_component_type_info() } -> std::same_as<std::meta::info>;
	    { T::get_component_pool_type_info() } -> std::same_as<std::meta::info>;
		{ T::get_component_bitset_type_info() } -> std::same_as<std::meta::info>;
		{ T::get_component_array_type_info() } -> std::same_as<std::meta::info>;
	}
	&& requires (T& t)
	{
		{ t.get_component_bitset() } -> std::same_as<typename [:T::get_component_bitset_type_info():]&>;
		{ t.get_component_array() } -> std::same_as<typename [:T::get_component_array_type_info():]&>;
	}
	&& requires (const T& t)
	{
		{ t.get_component_bitset() } -> std::same_as<const typename [:T::get_component_bitset_type_info():]&>;
		{ t.get_component_array() } -> std::same_as<const typename [:T::get_component_array_type_info():]&>;
	};
}
