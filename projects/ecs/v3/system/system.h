#pragma once

#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/utils/detail.h"
#include "ecs/v3/utils/types.h"
#include <bitset>
#include <meta>
#include <type_traits>
#include <vector>

namespace pulse::ecs
{
	template<
		typename FUNCTION_TYPE,
		FUNCTION_TYPE FUNCTION>
	struct System
	{
		consteval static std::meta::info get_function_type_info()
		{
			return ^^FUNCTION_TYPE;
		}

		consteval static std::vector<std::meta::info> get_parameter_type_infos()
		{
			const auto functionTypeInfo = get_function_type_info();
			const auto parameterInfos = std::meta::parameters_of(functionTypeInfo);
			return parameterInfos;
		}

		consteval static std::vector<std::meta::info> get_invoke_template_argument_infos(
			std::meta::info in_componentStoreInfo)
		{
			std::vector<std::meta::info> argumentInfos;
			argumentInfos.push_back(get_function_type_info());
			argumentInfos.push_back(in_componentStoreInfo);
			argumentInfos.insert_range(argumentInfos.end(), get_parameter_type_infos());
			return argumentInfos;
		}

		template<typename COMPONENT_STORE_TYPE>
		consteval static std::meta::info get_invoke_function_info()
		{
			constexpr auto invokeTemplateInfo = ^^pulse::ecs::utils::invoke;
			const auto invokeTemplateArgInfos = get_invoke_template_argument_infos(^^COMPONENT_STORE_TYPE);
			const auto invokeFunctionInfo = std::meta::substitute(invokeTemplateInfo, invokeTemplateArgInfos);
			return invokeFunctionInfo;
		}

		template<typename COMPONENT_STORE_TYPE>
		void invoke(
			std::add_lvalue_reference_t<COMPONENT_STORE_TYPE> in_componentStore)
		{
			constexpr auto invokeFunctionInfo = get_invoke_function_info<COMPONENT_STORE_TYPE>();
			[:invokeFunctionInfo:](m_function, in_componentStore);
		}

	private:
		std::add_pointer_t<FUNCTION_TYPE> m_function = FUNCTION;
	};
}
