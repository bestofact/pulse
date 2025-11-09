#pragma once

#include <array>
#include <meta>

namespace pulse::ecs
{
	template<typename _entity_storage_type, typename _entity_info_type, size_t _entity_capacity>
	consteval void define_entity_storage()
	{
		auto entities_member_info = std::meta::data_member_spec(
			std::meta::substitute(^^std::array, { ^^_entity_info_type, std::meta::reflect_constant(_entity_capacity) } ),
			{
				.name = "m_entities"
			}
		);

		std::meta::define_aggregate(^^_entity_storage_type, {entities_member_info});
	}
}