#pragma once

#include <cstddef>
#include <meta>

namespace pulse::ecs::__detail
{
	template<typename _ComponentType, std::size_t _ComponentIndex, std::ptrdiff_t _ComponentOffset>
	struct ComponentMeta
	{
		using ComponentType = _ComponentType;

		consteval static std::meta::info get_component_type_info() 
		{
			return ^^_ComponentType;
		}

		consteval static std::size_t get_component_index()
		{
			return _ComponentIndex;
		}

		consteval static std::meta::info get_component_index_info()
		{
			return std::meta::reflect_constant(get_component_index());
		}

		consteval static std::ptrdiff_t get_component_offset()
		{
			return _ComponentOffset;
		}

		consteval static std::meta::info get_component_offset_info()
		{
			return std::meta::reflect_constant(get_component_offset());
		}
	};

	consteval inline std::meta::info get_component_meta_template_info() 
	{
		return ^^ComponentMeta; 
	}
}