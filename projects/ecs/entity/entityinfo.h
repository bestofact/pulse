#pragma once

#include "ecs/core/utility.h"
#include <bitset>
#include <meta>


namespace pulse::ecs
{
    template<std::meta::info _entity_namespace_info, typename _entity_info_type>
	consteval void define_entity_info()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();

		// Get number of components for the entity.
    	size_t component_count = 0;
    	auto entity_namespace_member_infos = std::meta::members_of(_entity_namespace_info, ctx);
    	for(auto member_info : entity_namespace_member_infos)
    	{
    		if(pulse::ecs::utils::is_component(member_info))
    		{    			
    			++component_count;
    		}
    	}
        
        // Define a bitset<no_of_components> for the entity type.
		auto entity_bitmask_member_info = std::meta::data_member_spec(
		    std::meta::substitute(^^std::bitset, { std::meta::reflect_constant(component_count) }),
			{
			    .name = "m_component_bitset"
			}
		);

		std::meta::define_aggregate(^^_entity_info_type, { entity_bitmask_member_info });
	}
}
