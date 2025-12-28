#pragma once

#include "foundation/types.h"
#include <limits>

namespace pulse
{
	template<typename T>
	constexpr T min()
	{
		return std::numeric_limits<T>::min();
	}

	template<typename T> 
	constexpr T max()
	{
		return std::numeric_limits<T>::max();
	}

	constexpr u32 invalid_index() 
	{
		return max<u32>();
	}
}