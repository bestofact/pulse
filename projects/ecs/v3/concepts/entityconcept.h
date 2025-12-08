#pragma once

#include <concepts>
#include <cstddef>
#include <meta>

#include "ecs/v3/concepts/entitycapacityconcept.h"

namespace pulse::ecs::concepts
{

	template<typename T>
	concept Entity =
    std::is_class_v<T>
    && !std::is_empty_v<T>
    && (sizeof(T) > 0)
    && requires(T t)
    {
        typename T::CapacityType;
		requires pulse::ecs::concepts::EntityCapacity<typename T::CapacityType>;
        { T::s_capacity } -> std::convertible_to<typename T::CapacityType>;
    };
}
