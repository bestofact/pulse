#pragma once

#include <meta>

namespace pulse::ecs::__detail
{
	template<typename _FunctionType, _FunctionType _Function>
	struct SystemFunctionWrapper
	{
		consteval static std::meta::info get_function_type_info()
		{
			return ^^_FunctionType;
		}

		consteval static std::meta::info get_function_ptr_info()
		{
			return std::meta::add_pointer(get_function_type_info());
		}

		consteval static std::meta::info get_function_info()
		{
			return ^^_Function;
		}

		using FunctionType = _FunctionType;
		using FunctionPtrType = [:get_function_ptr_info():];

		FunctionPtrType m_functionPtr = _Function;
	};

	consteval inline std::meta::info get_system_function_wrapper_template_info()
	{
		return ^^SystemFunctionWrapper;
	}
}