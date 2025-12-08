#pragma once

#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/utils/detail.h"

#include <meta>
#include <array>
#include <bitset>

namespace pulse::ecs
{
    /*
     * Container for entity components.
     */
	template<pulse::ecs::concepts::Component COMPONENT_TYPE>
	struct ComponentPool final
	{
	    consteval static std::meta::info get_component_pool_template_info()
		{
		    return ^^ComponentPool;
		}

		consteval static std::meta::info get_component_type_info()
		{
			return ^^COMPONENT_TYPE;
		}

	    consteval static std::meta::info get_component_pool_type_info()
		{
		    return std::meta::substitute(get_component_pool_template_info(), {get_component_type_info()});
		}

		consteval static std::meta::info get_component_bitset_type_info()
		{
			const auto bitsetInfo = ^^std::bitset;
			const auto componentTypeInfo = get_component_type_info();
			const auto entityCapacityValueInfo = pulse::ecs::utils::get_entity_capacity_member_info(componentTypeInfo);
			return std::meta::substitute(bitsetInfo, {entityCapacityValueInfo});
		}

		consteval static std::meta::info get_component_array_type_info()
		{
			const auto arrayInfo = ^^std::array;
			const auto componentTypeInfo = get_component_type_info();
			const auto entityCapacityValueInfo = pulse::ecs::utils::get_entity_capacity_member_info(componentTypeInfo);
			return std::meta::substitute(arrayInfo, {componentTypeInfo, entityCapacityValueInfo});
		}

		consteval auto get_component_bitset() const
			-> const [:get_component_bitset_type_info():]&
		{
			return m_componentBitset;
		}

		consteval auto get_component_array() const
			-> const [:get_component_array_type_info():]&
		{
			return m_componentArray;
		}

		consteval auto get_component_bitset()
			-> [:get_component_bitset_type_info():]&
		{
			return m_componentBitset;
		}

		consteval auto get_component_array()
			-> [:get_component_array_type_info():]&
		{
			return m_componentArray;
		}

		[:get_component_bitset_type_info():] m_componentBitset;
		[:get_component_array_type_info():] m_componentArray;
	};

	// TODO (anil) : use a dummy component to instantiate ComponentWrapper<T>
	//static_assert(pulse::ecs::concepts::ComponentWrapper<ComponentWrapper>, "")
}
