#pragma once

#include <bit>
#include <cstddef>

#include <meta>
#include <type_traits>

namespace pulse::ecs
{
	struct EntityCapacity final
	{
		using ValueType = std::uint64_t;

		consteval static std::meta::info get_value_type_info()
		{
		    return ^^ValueType;
		}

		consteval EntityCapacity(const ValueType in_desiredEntityCount)
			: m_value((in_desiredEntityCount == 1) ? 1 : (1 << (64 - std::countl_zero(in_desiredEntityCount - 1))))
		{
		}

		consteval operator ValueType() const
		{
		    return m_value;
		}

		const ValueType m_value;
	};
}

namespace pulse::ecs::meta
{
	consteval std::meta::info get_entity_capacity_type_info()
	{
		return ^^EntityCapacity;
	}

	consteval std::meta::info get_entity_capacity_value_type_info()
	{
		return EntityCapacity::get_value_type_info();
	}
}
