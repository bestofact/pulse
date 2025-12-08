#pragma once

#include <concepts>
#include <cstddef>
#include <meta>
#include <type_traits>

namespace pulse::ecs::concepts
{
	template <typename T>
	concept EntityCapacity = std::is_integral_v<T> && std::is_unsigned_v<T>;
}
