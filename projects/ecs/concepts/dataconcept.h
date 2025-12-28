#pragma once

#include "ecs/concepts/helpers.h"

#include <meta>
#include <type_traits>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept Data =
	   pulse::ecs::concepts::RequireEcsAnyParentNamespace<T>
	&& std::is_class_v<T>
	&& requires 
	{
		// Type must be complete.
		sizeof(T);

		// Meta requirements.
    	requires []() consteval 
    	{
    		constexpr auto ctx = std::meta::access_context::unchecked();

    		// All non static data members must be public.
			auto members = std::meta::nonstatic_data_members_of(^^T, ctx);
			for(auto member : members)
			{
				if(!std::meta::is_public(member))
				{
					return false;
				}
			}

			return true;
    	}();
	};
}