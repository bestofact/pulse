#pragma once

#include <meta>
#include <cstddef>

#include "ecs/v3/utils/concepts.h"

#define meta_assert(expression, message) if(!(expression)) { throw message; }

namespace pulse::ecs::utils
{
	namespace __detail
	{
		consteval std::meta::info get_entity_capacity_member_info_from_entity_type_info(
			std::meta::info in_typeInfo)
		{
			meta_assert(is_entity_type(in_typeInfo), "Given type is not entity type.");

			std::meta::info result;
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::static_data_members_of(in_typeInfo, ctx);
			for(const auto member : members)
			{
				if(is_entity_capacity_type(std::meta::type_of(member)))
				{
					result = member;
					break;
				}
			}

			meta_assert(result != std::meta::info(), "Could not find entity capacity static member.");
			
			return result;
		}

		consteval std::meta::info get_entity_type_info_from_namespace_info(
			std::meta::info in_namespaceInfo)
		{
			meta_assert(in_namespaceInfo != std::meta::info(), "Given namespace info is invalid.");
			meta_assert(std::meta::is_namespace(in_namespaceInfo), "Given namespace info is not a namespace info");
		
			std::meta::info result;
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::members_of(in_namespaceInfo, ctx);
			for(const auto member : members)
			{
				if(is_entity_type(member))
				{
					result = member;
					break;
				}
			}

			meta_assert(result != std::meta::info(), "Could not find entity type in namespace.");
			return result;
		}

		consteval std::meta::info get_namespace_info_from_info(
			std::meta::info in_info)
		{
			meta_assert(in_info != std::meta::info(), "Given type info is invalid.");

			const auto parent = std::meta::parent_of(in_info);

			meta_assert(std::meta::is_namespace(parent), "Parent of given type is not namespace.");
			return parent;
		}

		consteval std::vector<std::meta::info> get_component_type_infos_from_namespace_info(
			std::meta::info in_namespaceInfo)
		{
			meta_assert(in_namespaceInfo != std::meta::info(), "Given namespace info is invalid.");
			meta_assert(std::meta::is_namespace(in_namespaceInfo), "Given namespace info is not a namespace info");
		
			std::vector<std::meta::info> componentTypeInfos;
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::members_of(in_namespaceInfo, ctx);
			for(const auto member : members)
			{
				if(is_component_type(member))
				{
					componentTypeInfos.push_back(member);
				}
			}

			return componentTypeInfos;
		}

		consteval bool can_access_parent_namespace(std::meta::info in_info)
		{
			return pulse::ecs::utils::is_system_function_info(in_info)
				|| pulse::ecs::utils::is_entity_type(in_info)
				|| pulse::ecs::utils::is_component_type(in_info);
		}
	}

	consteval std::meta::info get_namespace_info(std::meta::info in_info)
	{
		meta_assert(in_info != std::meta::info(), "Given info is invalid.");

		const auto typeInfo = __detail::can_access_parent_namespace(in_info) ? in_info : std::meta::type_of(in_info);
		meta_assert(typeInfo != std::meta::info(), "Type info couldn't retrieved to get namespace info.");
		
		return __detail::get_namespace_info_from_info(typeInfo);
	}

	consteval std::meta::info get_entity_type_info(std::meta::info in_info)
	{
		meta_assert(in_info != std::meta::info(), "Given type info is invalid.");

		if(std::meta::is_namespace(in_info))
		{
			return __detail::get_entity_type_info_from_namespace_info(in_info);
		}

		return get_entity_type_info(get_namespace_info(in_info));
	}

	consteval std::meta::info get_entity_capacity_member_info(std::meta::info in_info)
	{
		meta_assert(in_info != std::meta::info(), "Given info is invalid.");

		if(is_entity_type(in_info))
		{
			return __detail::get_entity_capacity_member_info_from_entity_type_info(in_info);
		}

		return get_entity_capacity_member_info(get_entity_type_info(in_info));
	}

	consteval std::vector<std::meta::info> get_component_type_infos(std::meta::info in_info)
	{
		meta_assert(in_info != std::meta::info(), "Given info is invalid.");

		if(std::meta::is_namespace(in_info))
		{
			return __detail::get_component_type_infos_from_namespace_info(in_info);
		}

		return get_component_type_infos(get_namespace_info(in_info));
	}

	template<
		typename FUNCTION_TYPE,
		typename COMPONENT_STORE_TYPE,
		typename... PARAMETER_TYPES>
	void invoke(
		std::add_pointer_t<FUNCTION_TYPE> in_function,
		std::add_lvalue_reference_t<COMPONENT_STORE_TYPE> in_componentStore)
	{
		// read entity masks for each component pool. make union of it.
		//	these are entity id's to iterate.
		const auto componentBitset = in_componentStore.template get_component_bitset<std::decay_t<PARAMETER_TYPES>...>();
		const auto entityCapacity = componentBitset.size();
		
		const auto getParameter = [&in_componentStore]<typename PARAMETER_TYPE>(std::decay_t<decltype(entityCapacity)> in_entityIndex)
		{
			[:pulse::ecs::utils::get_entity_type_info(std::meta::decay(^^PARAMETER_TYPE)):] entity;
			entity.set_index(in_entityIndex);

			if constexpr (std::is_const_v<PARAMETER_TYPE>)
			{
				return in_componentStore.template get_component<std::decay_t<PARAMETER_TYPE>>(entity);
			}

			return in_componentStore.template get_component_mutable<std::decay_t<PARAMETER_TYPE>>(entity);
		};

		for(std::decay_t<decltype(entityCapacity)> i = 0; i < entityCapacity; ++i)
		{
			if(componentBitset.test(i))
			{
				//invoke here
				std::invoke(
					in_function,
					getParameter.template operator()<PARAMETER_TYPES>(i)...//in_componentStore.template get_component<COMPONENT_TYPES>(entityIndex)...
				);
			}
		}
	}
}