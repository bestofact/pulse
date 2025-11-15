#pragma once

#include "ecs/detail/componentmeta.h"
#include "ecs/detail/componentmetaregistry.h"
#include "ecs/detail/systemfunctioninvoker.h"
#include "ecs/detail/systemfunctionwrapper.h"
#include "ecs/detail/systemfunctionmeta.h"
#include "ecs/detail/systemfunctionmetaregistry.h"
#include "ecs/detail/utility.h"

#include <meta>

namespace pulse::ecs
{
	template<std::meta::info _EntityNamespaceInfo, std::size_t _EntityCapacity>
	struct Scene
	{

		using Entity = std::size_t;

		struct EntityStore{};
		struct ComponentStore;
		struct SystemStore;

		consteval static std::meta::info get_entity_namespace_info() { return _EntityNamespaceInfo; }
		consteval static std::size_t get_entity_capacity() { return _EntityCapacity; }
		consteval static std::meta::info get_entity_capacity_info() { return std::meta::reflect_constant(get_entity_capacity()); }

		consteval static std::meta::info get_entity_type_info() { return ^^Entity; }
		consteval static std::meta::info get_entity_store_type_info() { return ^^EntityStore; }
		consteval static std::meta::info get_component_store_type_info() { return ^^ComponentStore; }
		consteval static std::meta::info get_system_store_type_info() { return ^^SystemStore; }

		consteval 
		{ 
			__detail::define_component_store(get_entity_namespace_info(), get_entity_capacity_info(), get_component_store_type_info()); 
			__detail::define_system_store(get_entity_namespace_info(), get_system_store_type_info(), __detail::get_system_function_wrapper_template_info());
		}

		using ComponentMetaRegistry = [:__detail::define_component_meta_registry(
			__detail::get_component_meta_template_info(),
			__detail::get_component_meta_registry_template_info(),
			get_component_store_type_info()
		):];
		consteval static std::meta::info get_component_meta_registry_type_info() { return ^^ComponentMetaRegistry; }

		using SystemFunctionMetaRegistry = [:__detail::define_system_function_meta_registry(
			get_entity_type_info(),
			get_component_store_type_info(),
			get_component_meta_registry_type_info(),
			get_system_store_type_info(),
			__detail::get_system_function_meta_template_info(), 
			__detail::get_system_function_meta_registry_template_info()
		):];
		consteval static std::meta::info get_system_function_meta_registry_type_info() { return ^^SystemFunctionMetaRegistry; }

	private:
		EntityStore m_entityStore;
		ComponentStore m_componentStore;
		SystemStore m_systemStore;

	public:
		const EntityStore& get_entity_store() { return m_entityStore; }
		const ComponentStore& get_component_store() { return m_componentStore; }
		const SystemStore& get_system_store() { return m_systemStore; }

		void invoke_systems(const Entity in_entity)
		{
			SystemFunctionMetaRegistry::invoke_sequential(in_entity, m_systemStore, m_componentStore);
		}
	};

}