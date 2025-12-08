#pragma once

#include <meta>

namespace pulse::ecs::concepts
{
	template <typename T>
	concept Component = 
	std::is_class_v<T>
	&& !std::is_empty_v<T>
	&& (sizeof(T) > 0)
	&& requires {
		// Parent must be namespace
    	requires std::meta::is_namespace(std::meta::parent_of(^^T));

    	// All non static data members must be public
    	requires []() consteval {
    		constexpr auto ctx = std::meta::access_context::unchecked();
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