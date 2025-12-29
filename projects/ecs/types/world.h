#pragma once

#include "ecs/concepts/archetypechunkconcept.h"
#include "ecs/concepts/archetypestoreconcept.h"
#include "ecs/concepts/entityconcept.h"
#include "ecs/concepts/systemstoreconcept.h"
#include "ecs/concepts/entitystoreconcept.h"
#include "ecs/concepts/outputconcept.h"
#include "ecs/concepts/componentconcept.h"

#include "ecs/utils/invokers.h"

#include "foundation/types.h"
#include "foundation/meta.h"

#include <meta>

namespace pulse::ecs
{
	template<
		pulse::ecs::concepts::EntityStore ENTITY_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore COMPONENT_STORE_TYPE,
		pulse::ecs::concepts::ArchetypeStore OUTPUT_STORE_TYPE,
		pulse::ecs::concepts::SystemStore SYSTEM_STORE_TYPE>
	struct World
	{
#pragma region ------------------- META ------------------

		consteval static std::meta::info get_entity_store_type()
		{
			return ^^ENTITY_STORE_TYPE;
		}

		consteval static std::meta::info get_component_store_type()
		{
			return ^^COMPONENT_STORE_TYPE;
		}

		consteval static std::meta::info get_output_store_type()
		{
			return ^^OUTPUT_STORE_TYPE;
		}

		consteval static std::meta::info get_system_store_type()
		{
			return ^^SYSTEM_STORE_TYPE;
		}

		consteval static std::meta::info get_entity_type()
		{
			return ENTITY_STORE_TYPE::get_entity_type();
		}

		consteval static u64 get_entity_capacity()
		{
			return ENTITY_STORE_TYPE::get_capacity();
		}

		template<
			pulse::ecs::concepts::ArchetypeStore ARCHETYPE_STORE_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		static const DATA_TYPE& get_data_in_archetype_store(
				const ARCHETYPE_STORE_TYPE& in_store, 
				const [:get_entity_type():] in_entity)
		{
			const auto& chunk = in_store.template get_archetype_chunk<DATA_TYPE>();
			const auto& data = chunk.template get_data<typename [:get_entity_type():], DATA_TYPE>(in_entity);
			return data;
		}

		template<
			pulse::ecs::concepts::ArchetypeStore ARCHETYPE_STORE_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		static DATA_TYPE& get_data_mutable_in_archetype_store(
				ARCHETYPE_STORE_TYPE& in_store, 
				const [:get_entity_type():] in_entity)
		{
			auto& chunk = in_store.template get_archetype_chunk_mutable<DATA_TYPE>();
			auto& data = chunk.template get_data_mutable<typename [:get_entity_type():], DATA_TYPE>(in_entity);
			return data;
		}

		template<
			pulse::ecs::concepts::ArchetypeStore ARCHETYPE_STORE_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		static bool test_data_occupation_in_archetype_store(
			const ARCHETYPE_STORE_TYPE& in_store,
			const [:get_entity_type():] in_entity)
		{
			const auto& chunk = in_store.template get_archetype_chunk<DATA_TYPE>();
			const bool result = chunk.template test_occupation<typename [:get_entity_type():], DATA_TYPE>(in_entity);
			return result;
		}

		template<
			pulse::ecs::concepts::ArchetypeStore ARCHETYPE_STORE_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		static void set_data_occupation_in_archetype_store(
			ARCHETYPE_STORE_TYPE& in_store,
			const [:get_entity_type():] in_entity)
		{
			auto& chunk = in_store.template get_archetype_chunk_mutable<DATA_TYPE>();
			chunk.template set_occupation<typename [:get_entity_type():], DATA_TYPE>(in_entity);
		}

		template<
			pulse::ecs::concepts::ArchetypeStore ARCHETYPE_STORE_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		static void reset_data_occupation_in_archetype_store(
			ARCHETYPE_STORE_TYPE& in_store,
			const [:get_entity_type():] in_entity)
		{
			auto& chunk = in_store.template get_archetype_chunk_mutable<DATA_TYPE>();
			chunk.template reset_occupation<typename [:get_entity_type():], DATA_TYPE>(in_entity);
		}
		
#pragma endregion

		[:get_entity_type():] new_entity()
		{
			return m_entityStore.new_entity();
		}

		void free_entity(const [:get_entity_type():] in_entity)
		{
			m_entityStore.free_entity(in_entity);

		}

		template<pulse::ecs::concepts::Component... COMPONENT_TYPE>
		void add_component(const [:get_entity_type():] in_entity)
		{
			(
				set_data_occupation_in_archetype_store<COMPONENT_STORE_TYPE, COMPONENT_TYPE>(
					m_componentStore, 
					in_entity
				), ...
			);
		}

		template<pulse::ecs::concepts::Component... COMPONENT_TYPE>
		void remove_component(const [:get_entity_type():] in_entity)
		{
			(
				reset_data_occupation_in_archetype_store<COMPONENT_STORE_TYPE, COMPONENT_TYPE>(
					m_componentStore, 
					in_entity
				), ...
			);
		}

		template<pulse::ecs::concepts::Component... COMPONENT_TYPES>
		bool has_component(const [:get_entity_type():] in_entity) const
		{
			return 
			(
				test_data_occupation_in_archetype_store<COMPONENT_STORE_TYPE, COMPONENT_TYPES>(
					m_componentStore,
					in_entity
				) && ...
			);
		}

		template<pulse::ecs::concepts::Output... OUTPUT_TYPES>
		bool has_output(const [:get_entity_type():] in_entity) const
		{
			return 
			(
				test_data_occupation_in_archetype_store<OUTPUT_STORE_TYPE, OUTPUT_TYPES>(
					m_outputStore,
					in_entity
				) && ...
			);
		}

		template<pulse::ecs::concepts::Component COMPONENT_TYPE>
		const COMPONENT_TYPE& get_component(const [:get_entity_type():] in_entity) const
		{
			return get_data_in_archetype_store<COMPONENT_STORE_TYPE, COMPONENT_TYPE>(m_componentStore, in_entity);
		}

		template<pulse::ecs::concepts::Component COMPONENT_TYPE>
		COMPONENT_TYPE& get_component_mutable(const [:get_entity_type():] in_entity)
		{
			return get_data_mutable_in_archetype_store<COMPONENT_STORE_TYPE, COMPONENT_TYPE>(m_componentStore, in_entity);
		}

		template<pulse::ecs::concepts::Output OUTPUT_TYPE>
		const OUTPUT_TYPE& get_output(const [:get_entity_type():] in_entity) const
		{
			return get_data_in_archetype_store<OUTPUT_STORE_TYPE, OUTPUT_TYPE>(m_outputStore, in_entity);
		}

		template<pulse::ecs::concepts::Component OUTPUT_TYPE>
		OUTPUT_TYPE& get_output_mutable(const [:get_entity_type():] in_entity)
		{
			return get_data_mutable_in_archetype_store<OUTPUT_STORE_TYPE, OUTPUT_TYPE>(m_outputStore, in_entity);
		}

		template<pulse::ecs::concepts::ArchetypeStore STORE_TYPE>
		void reset_occupations(STORE_TYPE& io_store)
		{
			template for(constexpr auto type : STORE_TYPE::get_archetype_chunk_types())
			{
				auto& chunk = io_store.template get_archetype_chunk_mutable<typename [:type:]>();
				chunk.reset_occupations();
			}
		}

		template<pulse::ecs::concepts::ArchetypeStore STORE_TYPE>
		void apply_occupations(STORE_TYPE& io_store)
		{
			template for(constexpr auto type : STORE_TYPE::get_archetype_chunk_types())
			{
				auto& chunk = io_store.template get_archetype_chunk_mutable<typename [:type:]>();
				chunk.apply_occupations();
			}
		}

		void invoke_systems()
		{
			reset_occupations(m_outputStore);
			apply_occupations(m_componentStore);

			pulse::ecs::invokers::invoke_systems<
				typename [:get_entity_type():],
			 	typename [:get_component_store_type():],
			 	typename [:get_output_store_type():],
			 	typename [:get_system_store_type():]
			 >(
			 	m_componentStore,
			 	m_outputStore
			 );

			 apply_occupations(m_outputStore);
			 apply_occupations(m_componentStore);
		}

	private:
		[:get_entity_store_type():] m_entityStore;
		[:get_component_store_type():] m_componentStore;
		[:get_output_store_type():] m_outputStore;
	};
}