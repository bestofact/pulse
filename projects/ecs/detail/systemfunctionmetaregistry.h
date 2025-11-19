#pragma once

#include <meta>

namespace pulse::ecs::__detail
{
	template<
		typename _EntityType,
		typename _ComponentStoreType,
		typename _ComponentMetaRegistryType,
		typename _SystemStoreType,
		typename ..._SystemFunctionMetaTypes>
	struct SystemFunctionMetaRegistry
	{
		template<typename _SystemFunctionMetaType>
		static void invoke(
			const _EntityType in_entity,
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore)
		{
			[:_SystemFunctionMetaType::build_invoker(^^_ComponentMetaRegistryType):]
			::invoke
			(
				in_entity, 
				in_systemStore,
				io_componentStore
			);
		}

		static void invoke_sequential(
			const _EntityType in_entity,
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore)
		{
			(invoke<_SystemFunctionMetaTypes>(in_entity, in_systemStore, io_componentStore), ...);
		}

		template<typename _SystemFunctionMetaType>
		static void invoke_query(
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore)
		{
			[:_SystemFunctionMetaType::build_invoker(^^_ComponentMetaRegistryType):]
			::template invoke_query<_EntityType, _EntityType::get_entity_capacity()>
			( 
				in_systemStore,
				io_componentStore
			);
		}

		static void invoke_sequential_query(
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore)
		{
			(invoke_query<_SystemFunctionMetaTypes>(in_systemStore, io_componentStore), ...);
		}
	};

	consteval inline std::meta::info get_system_function_meta_registry_template_info()
	{
		return ^^SystemFunctionMetaRegistry;
	}
}