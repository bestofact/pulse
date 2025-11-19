#pragma once

#include "ecs/detail/componentwrapper.h"
#include <cstddef>
#include <format>
#include <meta>
#include <string_view>

namespace pulse::ecs::__detail
{
	consteval inline std::size_t invalid_index() 
	{
		return static_cast<std::size_t>(-1); 
	}

	consteval inline std::ptrdiff_t invalid_offset()
	{
		return static_cast<std::ptrdiff_t>(-1);
	}

	consteval static bool is_component(std::meta::info in_info)
	{
		return std::meta::is_complete_type(in_info);
	}
	
	consteval static bool is_system(std::meta::info in_info)
	{
		return std::meta::is_function(in_info);
	}

	template<typename _EntityType>
	inline std::size_t get_entity_index(const _EntityType in_entity)
	{
		return in_entity.m_index;
	}

	template<
		typename _ComponentWrapperType,
		typename _ComponentStoreType,
		typename _ComponentMetaRegistryType>
	static std::decay_t<_ComponentWrapperType>& get_component_wrapper(
		_ComponentStoreType& in_componentStore)
	{
		constexpr auto componentWrapperTypeInfo = std::meta::decay(^^_ComponentWrapperType);
		constexpr auto componentTypeInfo = _ComponentWrapperType::get_component_type_info();
		constexpr auto componentWrapperOffset = _ComponentMetaRegistryType::get_component_wrapper_offset(componentTypeInfo);
	
		static_assert(componentWrapperOffset != invalid_offset());

		auto* storePtr = reinterpret_cast<char*>(&in_componentStore);
		auto* componentWrapperPtr = reinterpret_cast<[:componentWrapperTypeInfo:]*>(storePtr + componentWrapperOffset);
		return *componentWrapperPtr;
	}

	template<
		typename _EntityType,
		std::size_t _EntityCapacity,
		typename _ComponentType,
		typename _ComponentStoreType,
		typename _ComponentMetaRegistryType>
	static std::decay_t<_ComponentType>& get_component(
		const _EntityType in_entity, 
		_ComponentStoreType& in_componentStore)
	{
		constexpr auto componentTypeInfo = std::meta::decay(^^_ComponentType);
		constexpr auto entityCapacityInfo = std::meta::reflect_constant(_EntityCapacity);
		constexpr auto componentWrapperTypeInfo = get_component_wrapper_type_info(componentTypeInfo, entityCapacityInfo);
		
		auto& componentWrapper = get_component_wrapper<
			typename [:componentWrapperTypeInfo:],
			_ComponentStoreType,
			_ComponentMetaRegistryType>(
				in_componentStore
		);

		return componentWrapper.m_components[get_entity_index(in_entity)];
	}

	template<
		std::size_t _EntityCapacity,
		typename _ComponentType,
		typename _ComponentStoreType,
		typename _ComponentMetaRegistryType>
	static std::bitset<_EntityCapacity>& get_entity_bitset(
		_ComponentStoreType& in_componentStore)
	{
		constexpr auto componentTypeInfo = std::meta::decay(^^_ComponentType);
		constexpr auto entityCapacityInfo = std::meta::reflect_constant(_EntityCapacity);
		constexpr auto componentWrapperTypeInfo = get_component_wrapper_type_info(componentTypeInfo, entityCapacityInfo);
		
		auto& componentWrapper = get_component_wrapper<
			typename [:componentWrapperTypeInfo:],
			_ComponentStoreType,
			_ComponentMetaRegistryType>(
				in_componentStore
		);

		return componentWrapper.m_entityBitset;
	}

	consteval void define_component_store(
		std::meta::info in_entityNamespaceInfo,
		std::meta::info in_entityCapacityInfo,
		std::meta::info in_componentStoreTypeInfo)
	{
		std::vector<std::meta::info> componentStoreMemberInfos;

		const auto context = std::meta::access_context::unchecked();

		const auto members = std::meta::members_of(in_entityNamespaceInfo, context);
		for(auto member : members)
		{
			if(is_component(member))
			{
				const auto componentWrapperTypeInfo = get_component_wrapper_type_info(member, in_entityCapacityInfo);
				const auto componentIdentifier = std::meta::identifier_of(member);
				const auto componentWrapperMemberInfo = std::meta::data_member_spec(
					componentWrapperTypeInfo,
					{ .name = componentIdentifier }
				);

				componentStoreMemberInfos.push_back(componentWrapperMemberInfo);
			}
		}

		std::meta::define_aggregate(
			in_componentStoreTypeInfo,
			componentStoreMemberInfos
		);
	}

	consteval std::meta::info define_component_meta_registry(
		std::meta::info in_componentMetaTemplateInfo,
		std::meta::info in_componentMetaRegistryTemplateInfo,
		std::meta::info in_componentStoreTypeInfo)
	{
		std::size_t componentIndexCounter = 0;
		std::vector<std::meta::info> componentMetaRegistryMemberInfos;

		const auto context = std::meta::access_context::unchecked();

		const auto members = std::meta::members_of(in_componentStoreTypeInfo, context);
		for(const auto member : members)
		{
			if(std::meta::is_special_member_function(member))
			{
				continue;
			}

			const auto memberType = std::meta::type_of(member);
			const auto memberTemplate = std::meta::template_of(memberType);
			if(memberTemplate != get_component_wrapper_template_info())
			{
				continue;
			}

			const auto componentTypeInfo = std::meta::template_arguments_of(memberType)[0];
			
			const auto componentIndexInfo = std::meta::reflect_constant(componentIndexCounter++);
			
			const auto memberOffset = std::meta::offset_of(member);
			const auto memberOffsetInfo = std::meta::reflect_constant(memberOffset.bytes);
			
			const auto componentMetaTypeInfo = std::meta::substitute(
				in_componentMetaTemplateInfo,
				{ componentTypeInfo, componentIndexInfo, memberOffsetInfo }
			);

			componentMetaRegistryMemberInfos.push_back(componentMetaTypeInfo);
		}

		const auto componentMetaRegistryTypeInfo = std::meta::substitute(
			in_componentMetaRegistryTemplateInfo,
			componentMetaRegistryMemberInfos
		);
		
		return componentMetaRegistryTypeInfo;
	}

	static consteval void define_system_store(
		std::meta::info in_entityNamespaceInfo,
		std::meta::info in_systemStoreInfo,
		std::meta::info in_systemFunctionWrapperTemplateInfo)
	{
		std::vector<std::meta::info> systemStoreMemberInfos;

		const auto context = std::meta::access_context::unchecked();

		const auto members = std::meta::members_of(in_entityNamespaceInfo, context);
		for(auto member : members)
		{
			if(is_function(member))
			{
				const auto functionInfo = member;
				const auto functionTypeInfo = std::meta::type_of(functionInfo);
				const auto systemFunctionWrapperTypeInfo = std::meta::substitute(
					in_systemFunctionWrapperTemplateInfo, 
					{ functionTypeInfo, functionInfo }
				);

				const auto functionIdentifier = std::meta::identifier_of(functionInfo);
				const auto systemFunctionWrapperMemberInfo = std::meta::data_member_spec(
					systemFunctionWrapperTypeInfo,
					{ .name = functionIdentifier }
				);

				systemStoreMemberInfos.push_back(systemFunctionWrapperMemberInfo);
			}
		}

		std::meta::define_aggregate(
			in_systemStoreInfo,
			systemStoreMemberInfos
		);
	}

	consteval std::meta::info define_system_function_meta_registry(
		std::meta::info in_entityTypeInfo,
		std::meta::info in_componentStoreTypeInfo,
		std::meta::info in_componentMetaRegistryTypeInfo,
		std::meta::info in_systemStoreTypeInfo,
		std::meta::info in_systemFunctionMetaTemplateInfo,
		std::meta::info in_systemFunctionMetaRegistryTemplateInfo)
	{
		std::size_t functionIndexCounter = 0;
		std::vector<std::meta::info> systemFunctionMetaRegistryTemplateArgumentInfos;
		systemFunctionMetaRegistryTemplateArgumentInfos.push_back(in_entityTypeInfo);
		systemFunctionMetaRegistryTemplateArgumentInfos.push_back(in_componentStoreTypeInfo);
		systemFunctionMetaRegistryTemplateArgumentInfos.push_back(in_componentMetaRegistryTypeInfo);
		systemFunctionMetaRegistryTemplateArgumentInfos.push_back(in_systemStoreTypeInfo);

		const auto context = std::meta::access_context::unchecked();

		const auto members = std::meta::members_of(in_systemStoreTypeInfo, context);
		for(const auto member : members)
		{
			if(std::meta::is_special_member_function(member))
			{
				continue;
			}

			const auto systemFunctionWrapperMemberInfo = member;
			const auto systemFunctionWrapperTypeInfo = std::meta::type_of(systemFunctionWrapperMemberInfo);

			const auto functionIndexInfo = std::meta::reflect_constant(functionIndexCounter++);
			
			const auto systemFunctionWrapperOffset = std::meta::offset_of(systemFunctionWrapperMemberInfo);
			const auto systemFunctionWrapperOffsetInfo = std::meta::reflect_constant(systemFunctionWrapperOffset.bytes);
			
			const auto systemFunctionMetaTypeInfo = std::meta::substitute(
				in_systemFunctionMetaTemplateInfo,
				{ systemFunctionWrapperTypeInfo, functionIndexInfo, systemFunctionWrapperOffsetInfo }
			);

			systemFunctionMetaRegistryTemplateArgumentInfos.push_back(systemFunctionMetaTypeInfo);
		}

		const auto systemFunctionMetaRegistryTypeInfo = std::meta::substitute(
			in_systemFunctionMetaRegistryTemplateInfo,
			systemFunctionMetaRegistryTemplateArgumentInfos
		);
		return systemFunctionMetaRegistryTypeInfo;
		/**
		const auto system_function_utility_implementation_member_info = std::meta::data_member_spec(
			system_function_utility_implementation_type_info,
			{ .name = "m_system_function_utility_implementation" }
		);
		
		const auto system_function_utility_store_type_info = ^^_system_function_utility_store_type;
		std::meta::define_aggregate(
			system_function_utility_store_type_info,
			{ system_function_utility_implementation_member_info }
		);
		*/
	}
}