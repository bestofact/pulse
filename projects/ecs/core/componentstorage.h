#pragma once

#include "ecs/core/utility.h"

#include <meta>

namespace pulse::ecs
{
	template<std::meta::info _entity_namespace_info, typename _component_storage_type, size_t _entity_capacity>
	consteval void define_component_storage()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();

		std::vector<std::meta::info> component_storage_member_infos;

		auto member_infos = std::meta::members_of(_entity_namespace_info, ctx);
		for(auto member_info : member_infos)
		{
			if(pulse::ecs::utils::is_component(member_info))
			{
				auto component_storage_member_info = std::meta::data_member_spec(
					std::meta::substitute(^^std::array, { member_info, std::meta::reflect_constant(_entity_capacity)}),
					{
						.name = std::meta::identifier_of(member_info)
					}
				);

				component_storage_member_infos.push_back(component_storage_member_info);
			}
		}

		std::meta::define_aggregate(^^_component_storage_type, component_storage_member_infos);
	}
}