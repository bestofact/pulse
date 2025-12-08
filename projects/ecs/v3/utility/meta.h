#pragma once

#include "ecs/v3/entity/entitycapacity.h"

#include <cstddef>
#include <meta>
#include <array>
#include <bitset>
#include <vector>

namespace pulse::ecs::meta
{
	consteval std::meta::info invalid_info()
	{
		return std::meta::info();
	}
	
	consteval std::size_t invalid_index()
	{
		return static_cast<std::size_t>(-1);
	}

	consteval std::ptrdiff_t invalid_offset()
	{
		return static_cast<std::ptrdiff_t>(-1);
	}

	consteval bool is_all_nonstatic_data_members_are_public(
		std::meta::info in_typeInfo)
	{
		constexpr auto ctx = std::meta::access_context::unchecked();
		auto members = std::meta::nonstatic_data_members_of(in_typeInfo, ctx);

		for(auto member : members)
		{
			if(!std::meta::is_public(member))
			{
				return false;
			}
		}

		return true;
	}

	consteval bool is_component_type(
		const std::meta::info in_typeInfo)
	{
		return std::meta::is_type(in_typeInfo)
			&& std::meta::is_complete_type(in_typeInfo)
			&& !std::meta::is_empty_type(in_typeInfo)
			&& !std::meta::is_type_alias(in_typeInfo)
			&& std::meta::is_namespace(std::meta::parent_of(in_typeInfo))
			&& is_all_nonstatic_data_members_are_public(in_typeInfo);
	}

	consteval bool is_entity_capacity_type(
		const std::meta::info in_typeInfo)
	{
		return std::meta::is_type(in_typeInfo)
			&& std::meta::is_same_type(in_typeInfo, get_entity_capacity_type_info());
	}

	consteval std::meta::info get_entity_capacity_info_from_namespace_info(
		const std::meta::info in_namespaceInfo)
	{
		if(!std::meta::is_namespace(in_namespaceInfo))
		{
			return invalid_info();
		}

		constexpr auto ctx = std::meta::access_context::unchecked();
		const auto members = std::meta::static_data_members_of(in_namespaceInfo, ctx);
		if(members.empty())
		{
			return invalid_info();
		}

		auto entityCapacityMember = invalid_info();
		for(const auto member : members)
		{
			if(!is_entity_capacity_type(std::meta::type_of(member)))
			{
				continue;
			}

			entityCapacityMember = member;
		}

		return entityCapacityMember;
	}

	consteval std::meta::info get_entity_capacity_info(
		std::meta::info in_info)
	{
		if(in_info == invalid_info())
		{
			return invalid_info();
		}

		if(is_entity_capacity_type(in_info))
		{
			return in_info;
		}

		if(is_component_type(in_info))
		{
			return get_entity_capacity_info(std::meta::parent_of(in_info));
		}

		if(std::meta::is_namespace(in_info))
		{
			return get_entity_capacity_info_from_namespace_info(in_info);
		}

		if(!std::meta::is_type(in_info))
		{
			return get_entity_capacity_info(std::meta::type_of(in_info));
		}

		return invalid_info();
	}

	consteval auto get_entity_capacity(
		std::meta::info in_info) -> [:get_entity_capacity_type_info():]
	{
		const auto entityCapacityInfo = get_entity_capacity_info(in_info);
		const auto entityCapacity = std::meta::extract<typename [:get_entity_capacity_type_info():]>(entityCapacityInfo);
		return entityCapacity;
	}

	consteval auto get_entity_capacity_value(
		std::meta::info in_info) -> [:get_entity_capacity_value_type_info():]
	{
		constexpr auto ctx = std::meta::access_context::unchecked();

		const auto entityCapacityInfo = get_entity_capacity_info(in_info);
		const auto entityCapacityMembers = std::meta::nonstatic_data_members_of(std::meta::type_of(entityCapacityInfo), ctx);
		return std::meta::extract<typename [:get_entity_capacity_value_type_info():]>(entityCapacityMembers[0]);
	}
}