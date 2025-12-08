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

		consteval std::meta::info get_namespace_info_from_type_info(
			std::meta::info in_typeInfo)
		{
			meta_assert(in_typeInfo != std::meta::info(), "Given type info is invalid.");
			meta_assert(std::meta::is_type(in_typeInfo), "Given type info is not a type info.");

			const auto parent = std::meta::parent_of(in_typeInfo);

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
	}

	consteval std::meta::info get_namespace_info(std::meta::info in_info)
	{
		meta_assert(in_info != std::meta::info(), "Given info is invalid.");

		const auto typeInfo = std::meta::is_type(in_info) ? in_info : std::meta::type_of(in_info);
		meta_assert(typeInfo != std::meta::info(), "Type info couldn't retrieved to get namespace info.");
		
		return __detail::get_namespace_info_from_type_info(typeInfo);
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
}