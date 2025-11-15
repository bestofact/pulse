#pragma once

#include <cstddef>
#include <meta>

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

	consteval inline std::meta::info get_component_list_template_type_info()
	{
		return ^^std::array;
	}

	consteval inline std::meta::info get_component_list_type_info(
		std::meta::info in_componentTypeInfo, 
		std::meta::info in_componentCountInfo)
	{
		constexpr auto listInfo = get_component_list_template_type_info();
		return std::meta::substitute(listInfo, { in_componentTypeInfo, in_componentCountInfo });
	}

	template<
		typename _EntityType,
		typename _ComponentType,
		typename _ComponentStoreType,
		typename _ComponentMetaRegistryType>
	static std::decay_t<_ComponentType>& get_component(
		const _EntityType in_entity, 
		_ComponentStoreType& in_componentStore)
	{
		constexpr auto componentTypeInfo = std::meta::decay(^^_ComponentType);
		constexpr auto componentOffset = _ComponentMetaRegistryType::get_component_offset(componentTypeInfo);
		//constexpr auto componentListTypeInfo = get_component_list_type_info(componentTypeInfo, std::meta::reflect_constant(1000));
	
		static_assert(componentOffset != invalid_offset());

		auto* storePtr = reinterpret_cast<char*>(&in_componentStore);
		auto* component = reinterpret_cast<[:componentTypeInfo:]*>(storePtr + componentOffset + sizeof(_ComponentType) * static_cast<std::uint64_t>(in_entity));
		//auto& component = (*componentsPtrPtr)[in_entity];		
		
		return *component;
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
				const auto componentListInfo = get_component_list_type_info(member, in_entityCapacityInfo);
				const auto componentIdentifier = std::meta::identifier_of(member);
				const auto componentListMemberInfo = std::meta::data_member_spec(
					componentListInfo,
					{ .name = componentIdentifier }
				);

				componentStoreMemberInfos.push_back(componentListMemberInfo);
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

			const auto componentListMemberInfo = member;
			const auto componentListTypeInfo = std::meta::type_of(componentListMemberInfo);
			const auto componentTypeInfo = std::meta::template_arguments_of(componentListTypeInfo)[0];
			
			const auto componentIndexInfo = std::meta::reflect_constant(componentIndexCounter++);
			
			const auto componentListOffset = std::meta::offset_of(componentListMemberInfo);
			const auto componentListOffsetInfo = std::meta::reflect_constant(componentListOffset.bytes);
			
			const auto componentMetaTypeInfo = std::meta::substitute(
				in_componentMetaTemplateInfo,
				{ componentTypeInfo, componentIndexInfo, componentListOffsetInfo }
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