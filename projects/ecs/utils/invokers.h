#pragma once

#include "ecs/concepts/archetypechunkconcept.h"
#include "ecs/concepts/archetypestoreconcept.h"
#include "ecs/concepts/componentconcept.h"
#include "ecs/concepts/dataconcept.h"
#include "ecs/concepts/dataaccessconcept.h"
#include "ecs/concepts/entityconcept.h"
#include "ecs/concepts/entitystoreconcept.h"
#include "ecs/concepts/outputconcept.h"
#include "ecs/concepts/outputhandleconcept.h"
#include "ecs/concepts/systemconcept.h"
#include "ecs/concepts/systemchunkconcept.h"
#include "ecs/concepts/systemhandleconcept.h"
#include "ecs/concepts/systemstoreconcept.h"

#include "ecs/types/dataaccess.h"
#include "ecs/utils/meta.h"

#include "foundation/types.h"
#include "foundation/limits.h"

#include <bitset>
#include <initializer_list>
#include <meta>
#include <type_traits>
#include <utility>
#include <ranges>

#include <print>

namespace pulse::ecs::invokers
{
namespace __detail
{
	consteval static std::vector<std::meta::info> get_data_access_types(std::meta::info in_system)
	{
		constexpr auto accessTemplate = ^^pulse::ecs::DataAccess;

		std::vector<std::meta::info> types;
		const auto parameters = std::meta::parameters_of(in_system);
		for(const auto parameter : parameters)
		{
			const auto type = std::meta::is_function(in_system) ? std::meta::type_of(parameter) : parameter;
			const auto access = std::meta::substitute(accessTemplate, { type });
			types.push_back(access);
		}

		return types;
	}

	template<
		pulse::ecs::concepts::ArchetypeStore STORE_TYPE,
		pulse::ecs::concepts::DataAccess DATA_ACCESS_TYPE>
	auto get_data_occupation(
		STORE_TYPE& out_store)
	{
		const auto& chunk = out_store.template get_archetype_chunk<typename [:DATA_ACCESS_TYPE::get_data_type():]>();
		const auto occupation = chunk.template get_occupation<typename [:DATA_ACCESS_TYPE::get_data_type():]>();
		return occupation;
	}

	template<
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::DataAccess DATA_ACCESS_TYPE>
	requires (pulse::ecs::meta::is_output(DATA_ACCESS_TYPE::get_data_type()))
	auto get_data_occupation(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		return get_data_occupation<OUTPUT_STORE_TYPE, DATA_ACCESS_TYPE>(out_outputStore);
	}

	template<
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::DataAccess DATA_ACCESS_TYPE>
	requires (pulse::ecs::meta::is_component(DATA_ACCESS_TYPE::get_data_type()))
	auto get_data_occupation(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		return get_data_occupation<COMPONENT_STORE_TYPE, DATA_ACCESS_TYPE>(out_componentStore);
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore STORE_TYPE,
		pulse::ecs::concepts::DataAccess DATA_ACCESS_TYPE>
	[:DATA_ACCESS_TYPE::get_type():] access_data(
		const ENTITY_TYPE in_entity,
		STORE_TYPE& out_store)
	{
		constexpr auto type = DATA_ACCESS_TYPE::get_data_type();
		if constexpr(DATA_ACCESS_TYPE::is_mutable())
		{
			auto& chunk = out_store.template get_archetype_chunk_mutable<typename [:type:]>();
			auto& data = chunk.template get_data_mutable<ENTITY_TYPE, typename [:type:]>(in_entity);
			return data;
		}

		const auto& chunk = out_store.template get_archetype_chunk<typename [:type:]>();
		const auto& data = chunk.template get_data<ENTITY_TYPE, typename [:type:]>(in_entity);
		return data;
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::DataAccess DATA_ACCESS_TYPE>
	requires (pulse::ecs::meta::is_output(DATA_ACCESS_TYPE::get_data_type()))
	[:DATA_ACCESS_TYPE::get_type():] access_data(
		const ENTITY_TYPE in_entity,
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		return access_data<ENTITY_TYPE, OUTPUT_STORE_TYPE, DATA_ACCESS_TYPE>(
			in_entity,
			out_outputStore
		);
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::DataAccess DATA_ACCESS_TYPE>
	requires (pulse::ecs::meta::is_component(DATA_ACCESS_TYPE::get_data_type()))
	[:DATA_ACCESS_TYPE::get_type():] access_data(
		const ENTITY_TYPE in_entity,
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		return access_data<ENTITY_TYPE, COMPONENT_STORE_TYPE, DATA_ACCESS_TYPE>(
			in_entity,
			out_componentStore
		);
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::System SYSTEM_TYPE,
		pulse::ecs::concepts::OutputHandle OUTPUT_HANDLE_TYPE,
		pulse::ecs::concepts::DataAccess... DATA_ACCESS_TYPES>
	void invoke_system_impl(
		std::add_pointer_t<SYSTEM_TYPE> in_system,
		const ENTITY_TYPE in_entity,
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		const OUTPUT_HANDLE_TYPE handle = in_system(
			__detail::access_data<
				ENTITY_TYPE, 
				COMPONENT_STORE_TYPE,
				OUTPUT_STORE_TYPE, 
				DATA_ACCESS_TYPES
			>(
				in_entity,
				out_componentStore,
				out_outputStore
			)...
		);

		constexpr auto count = OUTPUT_HANDLE_TYPE::get_output_count();
		template for(constexpr pulse::u64 index : std::views::iota((pulse::u64)0, count))
		{
			constexpr auto type = OUTPUT_HANDLE_TYPE::get_nth_output_type(index);
			if(handle.template has_result<typename [:type:]>())
			{
				auto& chunk = out_outputStore.template get_archetype_chunk_mutable<typename [:type:]>();
				auto& data = chunk.template get_data_mutable<ENTITY_TYPE, typename [:type:]>(in_entity);
				data = handle.template get_result<typename [:type:]>();
			}
		}
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::System SYSTEM_TYPE,
		typename RETURN_TYPE,
		pulse::ecs::concepts::DataAccess... DATA_ACCESS_TYPES>
	void invoke_system_impl(
		std::add_pointer_t<SYSTEM_TYPE> in_system,
		const ENTITY_TYPE in_entity,
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		in_system(
			__detail::access_data<
				ENTITY_TYPE, 
				COMPONENT_STORE_TYPE,
				OUTPUT_STORE_TYPE, 
				DATA_ACCESS_TYPES
			>(
				in_entity,
				out_componentStore,
				out_outputStore
			)...
		);
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemHandle SYSTEM_HANDLE_TYPE,
		pulse::ecs::concepts::DataAccess... DATA_ACCESS_TYPES>
	void invoke_system(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		std::bitset<ENTITY_TYPE::get_capacity()> entities;
		entities.set();
		((entities &= get_data_occupation<COMPONENT_STORE_TYPE, OUTPUT_STORE_TYPE, DATA_ACCESS_TYPES>(out_componentStore, out_outputStore)), ...);
	
		ENTITY_TYPE entity;
		for(pulse::u64 index = 0; index < ENTITY_TYPE::get_capacity(); ++index)
		{
			if(!entities.test(index))
			{
				continue;
			}

			entity.set_index(index);
			invoke_system_impl<
				ENTITY_TYPE,
				COMPONENT_STORE_TYPE,
				OUTPUT_STORE_TYPE, 
				typename [:SYSTEM_HANDLE_TYPE::get_system_type():], 
				typename [:SYSTEM_HANDLE_TYPE::get_return_type():], 
				DATA_ACCESS_TYPES...
			>(
				SYSTEM_HANDLE_TYPE::get_system_instance(), 
				entity, 
				out_componentStore, 
				out_outputStore
			);
		}
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemHandle SYSTEM_HANDLE_TYPE>
	consteval std::meta::info get_invoke_system_function()
	{
		std::vector<std::meta::info> args;
		args.push_back(^^ENTITY_TYPE);
		args.push_back(^^COMPONENT_STORE_TYPE);
		args.push_back(^^OUTPUT_STORE_TYPE);
		args.push_back(^^SYSTEM_HANDLE_TYPE);
		args.append_range(get_data_access_types(SYSTEM_HANDLE_TYPE::get_system_type()));

		constexpr auto functionTemplate = ^^pulse::ecs::invokers::__detail::invoke_system;
		const auto function = std::meta::substitute(functionTemplate, args);
		return function;
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemHandle SYSTEM_HANDLE_TYPE>
	void invoke_system_handle(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		constexpr auto function = __detail::get_invoke_system_function<ENTITY_TYPE, COMPONENT_STORE_TYPE, OUTPUT_STORE_TYPE, SYSTEM_HANDLE_TYPE>();
		[:function:](out_componentStore, out_outputStore);
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemChunk SYSTEM_CHUNK_TYPE>
	void invoke_system_chunk(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		template for(constexpr auto type : SYSTEM_CHUNK_TYPE::get_system_handle_types())
		{
			invoke_system_handle<ENTITY_TYPE, COMPONENT_STORE_TYPE, OUTPUT_STORE_TYPE, typename [:type:]>(out_componentStore, out_outputStore);
		}
	}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemStore SYSTEM_STORE_TYPE>
	void invoke_system_store(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		template for(constexpr auto type : SYSTEM_STORE_TYPE::get_system_chunk_types())
		{
			invoke_system_chunk<ENTITY_TYPE, COMPONENT_STORE_TYPE, OUTPUT_STORE_TYPE, typename [:type:]>(out_componentStore, out_outputStore);
		}
	}
}

	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemStore SYSTEM_STORE_TYPE>
	void invoke_systems(
		COMPONENT_STORE_TYPE& out_componentStore,
		OUTPUT_STORE_TYPE& out_outputStore)
	{
		__detail::invoke_system_store<ENTITY_TYPE, COMPONENT_STORE_TYPE, OUTPUT_STORE_TYPE, SYSTEM_STORE_TYPE>(out_componentStore, out_outputStore);
	}
}