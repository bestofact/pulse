#pragma once

#include "ecs/detail/utility.h"

#include <meta>

namespace pulse::ecs::__detail
{
	template<
		typename _ComponentMetaRegistryType,
		typename _SystemFunctionWrapperType,
		std::size_t _SystemFunctionOffset,
		typename ..._FunctionParamTypes>
	struct SystemFunctionInvoker
	{
		template<typename _EntityType, typename _SystemStoreType, typename _ComponentStoreType>
		static void invoke(
			const _EntityType in_entity,
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore)
		{
			// Locate function wrapper using offset
			const auto* storePtr = reinterpret_cast<const char*>(&in_systemStore);
			const auto* wrapperPtr = reinterpret_cast<const _SystemFunctionWrapperType*>(storePtr + _SystemFunctionOffset);
		
			// Call function with components as arguments
			std::invoke(
				wrapperPtr->m_functionPtr,
				get_component<_EntityType, _FunctionParamTypes, _ComponentStoreType, _ComponentMetaRegistryType>(in_entity, io_componentStore)...
			);
		}
	};

	consteval inline std::meta::info get_system_function_invoker_template_info()
	{
		return ^^SystemFunctionInvoker;
	}
}