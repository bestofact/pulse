#pragma once

#include "ecs/detail/utility.h"
#include <cstddef>
#include <meta>

#define WHEN_COMPONENT_META_MATCH(TestComponentTypeInfo, Body)\
	((std::meta::decay(TestComponentTypeInfo) == _ComponentMetas::get_component_type_info()\
	 ? (Body), true\
	 : false)\
	|| ...\
	)

namespace pulse::ecs::__detail
{

	/*
	 Runtime registry for component metadata lookups
	 Provides O(1) compile-time lookups via fold expressions
	*/
	template<typename... _ComponentMetas>
	struct ComponentMetaRegistry
	{
		consteval static std::meta::info get_component_meta_info(std::meta::info in_component_type_info)
		{
			std::meta::info result = std::meta::info();
			WHEN_COMPONENT_META_MATCH(in_component_type_info, result = ^^_ComponentMetas);
			return result;
		}

		consteval static std::size_t get_component_index(std::meta::info in_component_type_info)
		{
			std::size_t result = invalid_index();
			WHEN_COMPONENT_META_MATCH(in_component_type_info, result = _ComponentMetas::get_component_index());
			return result;
		}

		consteval static std::meta::info get_component_index_info(std::meta::info in_component_type_info)
		{
			std::meta::info result = std::meta::info();
			WHEN_COMPONENT_META_MATCH(in_component_type_info, result = _ComponentMetas::get_component_index_info());
			return result;
		}

		consteval static std::ptrdiff_t get_component_offset(std::meta::info in_component_type_info)
		{
			std::ptrdiff_t result = invalid_offset();
			WHEN_COMPONENT_META_MATCH(in_component_type_info, result = _ComponentMetas::get_component_offset());
			return result;
		}

		consteval static std::meta::info get_component_offset_info(std::meta::info in_component_type_info)
		{
			std::meta::info result = std::meta::info();
			WHEN_COMPONENT_META_MATCH(in_component_type_info, result = _ComponentMetas::get_component_offset_info());
			return result;
		}
	};


	consteval inline std::meta::info get_component_meta_registry_template_info()
	{
		return ^^ComponentMetaRegistry;
	}
}

#undef WHEN_COMPONENT_META_MATCH