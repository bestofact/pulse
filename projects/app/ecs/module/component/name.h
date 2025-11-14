#pragma once

#include <string>

namespace pulse::ecs::module
{
	struct Name
	{
		std::string m_name = std::string();
	};

	struct Fame
	{
		int m_lvl = 0;
	};
}