#pragma once

#include "ecs/concepts/dataconcept.h"

#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept DataAccess = 
	requires
	{
		{ T::get_type() } -> std::same_as<std::meta::info>;
		{ T::get_data_type() } -> std::same_as<std::meta::info>;
		{ T::is_mutable() } -> std::same_as<bool>;
		{ T::is_optional() } -> std::same_as<bool>;

		pulse::ecs::concepts::Data<typename [:T::get_data_type():]>;
	};
}