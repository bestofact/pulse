#pragma once

#include <bitset>
#include <meta>

namespace pulse::ecs
{
	template<typename _ComponentType, std::size_t _EntityCapacity>
	struct ComponentWrapper
	{
		using ComponentType = _ComponentType;

		consteval static std::meta::info get_component_type_info()
		{
			return ^^_ComponentType;
		}

		consteval static std::size_t get_entity_capacity()
		{
			return _EntityCapacity;
		}

		consteval static std::meta::info get_entity_capacity_info()
		{
			return std::meta::reflect_constant(get_entity_capacity());
		}


		ComponentWrapper()
		{
			m_entityBitset.reset();
		}

		std::bitset<_EntityCapacity> m_entityBitset;
		std::array<_ComponentType, _EntityCapacity> m_components;
	};

	consteval static std::meta::info get_component_wrapper_template_info()
	{
		return ^^ComponentWrapper;
	}

	consteval static std::meta::info get_component_wrapper_type_info(
		std::meta::info in_componentTypeInfo,
		std::meta::info in_entityCapacityInfo)
	{
		return std::meta::substitute(
			get_component_wrapper_template_info(),
			{ in_componentTypeInfo, in_entityCapacityInfo}
		);
	}
}