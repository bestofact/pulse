#pragma once

#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/utils/detail.h"
#include "ecs/v3/utils/types.h"
#include <bitset>
#include <meta>
#include <type_traits>
#include <vector>


namespace pulse::ecs::utils
{
	template<
		typename FUNCTION_TYPE,
		typename COMPONENT_STORE_TYPE,
		typename... COMPONENT_TYPES>
	void invoke(
		std::add_pointer_t<FUNCTION_TYPE> in_function,
		std::add_lvalue_reference_t<std::add_const_t<COMPONENT_STORE_TYPE>> in_componentStore)
	{
		// read entity masks for each component pool. make union of it. 
		//	these are entity id's to iterate.
		constexpr auto componentCount = sizeof...(COMPONENT_TYPES);
		constexpr auto entityCapacity = std::meta::extract<std::size_t>(
				pulse::ecs::utils::get_entity_capacity_member_info(^^FUNCTION_TYPE)
		);

		std::bitset<entityCapacity> entities;
		entities.set();

		const auto maskEntities = [&in_componentStore, &entities]<pulse::ecs::concepts::Component COMPONENT_TYPE>()
		{
			const auto& pool = in_componentStore.template get_component_pool<COMPONENT_TYPE>();
			const std::bitset<entityCapacity> entityMask = pool.get_component_bitset();

			entities &= entityMask;
		};
		(maskEntities.template operator()<COMPONENT_TYPES>(), ...);

		// for each entity in the component store invoke the function.
		const auto getComponent = [&in_componentStore]<pulse::ecs::concepts::Component COMPONENT_TYPE>(const decltype(entityCapacity) in_entityIndex)
		{
			const auto& pool = in_componentStore.template get_component_pool<COMPONENT_TYPE>();
			const auto& array = pool.get_comopnent_array();

			return array[in_entityIndex];
		};

		for(decltype(entityCapacity) i = 0; i < entityCapacity; ++i)
		{
			if(entities.test(i))
			{
				//invoke here
				const auto entityIndex = i;
				std::invoke(
					in_function,
					getComponent.template operator()<COMPONENT_TYPES>(entityIndex)...
				);
			}
		}
	}
}




namespace pulse::ecs
{

	template<
		typename FUNCTION_TYPE, 
		FUNCTION_TYPE FUNCTION>
	struct System
	{
		consteval std::meta::info get_function_type_info()
		{
			return ^^FUNCTION_TYPE;
		}

		consteval std::vector<std::meta::info> get_parameter_type_infos()
		{
			const auto functionTypeInfo = get_function_type_info();
			const auto parameterInfos = std::meta::parameters_of(functionTypeInfo);
			return parameterInfos;
		}

		consteval std::vector<std::meta::info> get_invoke_template_argument_infos(
			std::meta::info in_componentStoreInfo)
		{
			std::vector<std::meta::info> argumentInfos;
			argumentInfos.push_back(^^FUNCTION_TYPE);
			argumentInfos.push_back(in_componentStoreInfo);
			argumentInfos.insert_range(argumentInfos.end(), get_parameter_type_infos());
			return argumentInfos;
		}

		template<typename COMPONENT_STORE_TYPE>
		consteval std::meta::info get_invoke_function_info()
		{
			constexpr auto paramterInfos = get_parameter_type_infos();
			constexpr auto invokeTemplateInfo = ^^pulse::ecs::utils::invoke;
			constexpr auto invokeFunctionInfo = std::meta::substitute(invokeTemplateInfo,get_invoke_template_argument_infos(^^COMPONENT_STORE_TYPE));
			return invokeFunctionInfo;
		}

		template<typename COMPONENT_STORE_TYPE>
		void invoke(
			std::add_lvalue_reference_t<std::add_const_t<COMPONENT_STORE_TYPE>> in_componentStore)
		{
			//constexpr auto invokeFunctionInfo = get_invoke_function_info<COMPONENT_STORE_TYPE>();
			//[:invokeFunctionInfo:](m_function, in_componentStore);
		}

	private:
		std::add_pointer_t<FUNCTION_TYPE> m_function = FUNCTION;
	};
}