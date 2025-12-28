#pragma once


#include "ecs/concepts/systemhandleconcept.h"

namespace pulse::ecs::concepts
{
namespace __detail
{
	template<
		pulse::ecs::concepts::SystemHandle PRODUCER, 
		pulse::ecs::concepts::SystemHandle CONSUMER>
	consteval bool check_system_handle_dependency()
	{
		const auto producedTypes = PRODUCER::get_produced_output_types();
		const auto consumedTypes = CONSUMER::get_consumed_output_types();

		for(const auto consumed : consumedTypes)
		{
			for(const auto produced : producedTypes)
			{
				if(std::meta::is_same_type(produced, consumed))
				{
					return true;
				}
			}
		}
		
		return false;
	}
}

	template<typename PRODUCER, typename CONSUMER>
	concept HasDependency = 
	   pulse::ecs::concepts::SystemHandle<PRODUCER>
	&& pulse::ecs::concepts::SystemHandle<CONSUMER>
	&& __detail::check_system_handle_dependency<PRODUCER, CONSUMER>();
}