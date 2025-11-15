#pragma once

#include "ecs/detail/systemfunctioninvoker.h"
#include <cstddef>
#include <meta>

namespace pulse::ecs::__detail
{

	template<typename _SystemFunctionWrapperType, std::size_t _FunctionIndex, std::ptrdiff_t _FunctionOffset>
	struct SystemFunctionMeta
	{
		consteval static std::meta::info get_system_function_wrapper_type_info()
		{
			return ^^_SystemFunctionWrapperType;
		}

		consteval static std::meta::info get_function_type_info()
		{
			return _SystemFunctionWrapperType::get_function_type_info();
		}

		consteval static std::meta::info get_function_ptr_type_info()
		{
			return _SystemFunctionWrapperType::get_function_ptr_type_info();
		}

		consteval static std::meta::info get_function_info()
		{
			return _SystemFunctionWrapperType::get_function_info();
		}

		consteval static std::size_t get_function_index()
		{
			return _FunctionIndex;
		}

		consteval static std::meta::info get_function_index_info()
		{
			return std::meta::reflect_constant(get_function_index());
		}

		consteval static std::ptrdiff_t get_function_offset()
		{
			return _FunctionOffset;
		}

		consteval static std::meta::info get_function_offset_info()
		{
			return std::meta::reflect_constant(get_function_offset());
		}

		consteval static std::meta::info build_invoker(std::meta::info in_componentMetaRegistryType)
		{
			std::vector<std::meta::info> invokerTemplateArgInfos;
			invokerTemplateArgInfos.push_back(in_componentMetaRegistryType);
			invokerTemplateArgInfos.push_back(get_system_function_wrapper_type_info());
			invokerTemplateArgInfos.push_back(get_function_offset_info());

			const auto functionParamInfos = std::meta::parameters_of(get_function_type_info());
			for(const auto functionParamInfo : functionParamInfos)
			{
				invokerTemplateArgInfos.push_back(functionParamInfo);
			}

			const auto invokerInfo = std::meta::substitute(
				get_system_function_invoker_template_info(),
				invokerTemplateArgInfos
			);

			return invokerInfo;
		}
	};

	consteval inline std::meta::info get_system_function_meta_template_info()
	{
		return ^^SystemFunctionMeta;
	}
}