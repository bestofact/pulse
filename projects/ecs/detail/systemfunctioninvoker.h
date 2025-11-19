#pragma once

#include "ecs/detail/componentwrapper.h"
#include "ecs/detail/utility.h"

#include <bitset>
#include <meta>
#include <print>

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
			constexpr std::size_t entityCapaicty = _EntityType::get_entity_capacity();
			const auto* systemFunctionStorePtr = reinterpret_cast<const char*>(&in_systemStore);
			const auto* systemFunctionWrapperPtr = reinterpret_cast<const _SystemFunctionWrapperType*>(systemFunctionStorePtr + _SystemFunctionOffset);
		
			// Call function with components as arguments
			std::invoke(
				systemFunctionWrapperPtr->m_functionPtr,
				get_component<_EntityType, entityCapaicty, _FunctionParamTypes, _ComponentStoreType, _ComponentMetaRegistryType>(in_entity, io_componentStore)...
			);
		}

		// TODO(anil) Not the best place to put those query stuff, will do the job for now.
		template<
			std::size_t _EntityCapacity, 
			typename _ComponentType,
			typename _SystemStoreType,
			typename _ComponentStoreType>
		static void overlap_entity_bitset(
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore,
			std::bitset<_EntityCapacity>& io_entityBitset)
		{
			constexpr auto componentTypeInfo = ^^_ComponentType; 
			constexpr auto entityCapacityInfo = std::meta::reflect_constant(_EntityCapacity);
			constexpr auto componentWrapperTypeInfo = get_component_wrapper_type_info(componentTypeInfo, entityCapacityInfo);
			
			const auto entityBitset = __detail::get_entity_bitset<_EntityCapacity, _ComponentType, _ComponentStoreType, _ComponentMetaRegistryType>(io_componentStore);
			io_entityBitset = io_entityBitset & entityBitset;
		}

		template<
			typename _EntityType, 
			std::size_t _EntityCapacity,
			typename _SystemStoreType,
			typename _ComponentStoreType>
		static void invoke_query(
			const _SystemStoreType& in_systemStore,
			_ComponentStoreType& io_componentStore)
		{
			std::bitset<_EntityCapacity> queriedEntities;
			queriedEntities.set();
			(overlap_entity_bitset<
				_EntityCapacity, 
				std::decay_t<_FunctionParamTypes>, 
				_SystemStoreType,
				_ComponentStoreType>(
					in_systemStore, 
					io_componentStore, 
					queriedEntities
				),
				...
			);

			for(std::size_t index = 0; index < queriedEntities.count(); ++index)
			{
				if(queriedEntities.test(index))
				{
					invoke(_EntityType{index}, in_systemStore, io_componentStore);
				}
			}
		}
	};

	consteval inline std::meta::info get_system_function_invoker_template_info()
	{
		return ^^SystemFunctionInvoker;
	}
}