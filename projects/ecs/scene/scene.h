#pragma once
#include "ecs/component/componentstorage.h"
#include "ecs/component/componentindexer.h"
#include "ecs/system/systemstorage.h"
#include "ecs/system/systemexecutorsequential.h"
#include "ecs/entity/entityinfo.h"
#include "ecs/entity/entitystorage.h"

#include <meta>

namespace pulse::ecs
{
	template<std::meta::info _entity_namespace_info, size_t _entity_capacity>
	struct Scene
	{
		struct EntityInfoType;
		struct EntityStorage;
	    struct ComponentStorage;
	    struct ComponentIndexer;
	    struct SystemStorage;
	    struct SystemExecutor;

		static constexpr inline size_t s_entity_capacity = _entity_capacity;

		consteval
		{
			constexpr auto entity_capacity = s_entity_capacity;

		    pulse::ecs::define_component_storage<_entity_namespace_info, ComponentStorage, entity_capacity>();
		    pulse::ecs::define_system_storage<_entity_namespace_info, SystemStorage>();
		    pulse::ecs::define_system_executor_sequential<^^SystemStorage, SystemExecutor>();

		    pulse::ecs::define_component_indexer<^^ComponentStorage, ComponentIndexer>();
			pulse::ecs::define_entity_info<_entity_namespace_info, EntityInfoType>();
		    pulse::ecs::define_entity_storage<EntityStorage, EntityInfoType, entity_capacity>();
		}

	public:
		template<typename _component_type>
		constexpr size_t get_component_index()
		{
			return decltype(m_component_indexer.m_component_indexer)::template get_component_index<_component_type>();
		}

		template<typename _component_type>
		void add_component(const size_t in_entity_index)
		{
			const auto component_index = get_component_index<_component_type>();
			m_entity_storage.m_entities[in_entity_index].m_component_bitset.set(component_index);
		}

		template<typename _component_type>
		void remove_component(const size_t in_entity_index)
		{
			const auto component_index = get_component_index<_component_type>();
			m_entity_storage.m_entities[in_entity_index].m_component_bitset.reset(component_index);
		}

		template<typename _component_type>
		bool has_component(const size_t in_entity_index)
		{
			const auto component_index = get_component_index<_component_type>();
			return m_entity_storage.m_entities[in_entity_index].m_component_bitset[component_index];
		}

	public:
		EntityStorage m_entity_storage;
		ComponentStorage m_component_storage;
		ComponentIndexer m_component_indexer;
		SystemStorage m_system_storage;
		SystemExecutor m_system_executor;
	};
}