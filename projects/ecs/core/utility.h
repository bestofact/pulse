#pragma once
#include <meta>

namespace pulse::ecs::utils
{
	consteval bool is_system(std::meta::info in_info)
	{
		return std::meta::is_function(in_info);
	}

	consteval bool is_component(std::meta::info in_info)
	{
		return std::meta::is_type(in_info);
	}
}