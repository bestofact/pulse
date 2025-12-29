#pragma once

#include "ecs/concepts/archetypechunkconcept.h"
#include "ecs/concepts/archetypestoreconcept.h"
#include "ecs/concepts/dataconcept.h"

#include "ecs/concepts/entityconcept.h"
#include "foundation/meta.h"
#include "foundation/types.h"

#include <meta>

namespace pulse::ecs
{
	template<
		pulse::ecs::concepts::Entity ENTITY_TYPE,
		pulse::ecs::concepts::ArchetypeStore STORE_TYPE>
	struct ArchetypeModifier
	{
#pragma region ----------------- META ------------------
		consteval static std::meta::info get_entity_type()
		{
			return ^^ENTITY_TYPE;
		}

		consteval static std::meta::info get_store_type()
		{
			return ^^STORE_TYPE;
		}
#pragma endregion
		ArchetypeModifier(
			const [:get_entity_type():] in_entity,
			[:get_store_type():]& io_store
		)
			: m_entity(in_entity)
			, m_store(io_store)
		{}

	private:
		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		void set_data_value(const TARGET_DATA_TYPE& in_data)
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			auto& data = chunk.template get_data_mutable<typename [:get_entity_type():], TARGET_DATA_TYPE>(m_entity);
			data = in_data;
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		void set_occupation()
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			chunk.template set_occupation<typename [:get_entity_type():], TARGET_DATA_TYPE>(m_entity);
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		void reset_occupation()
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			chunk.template reset_occupation<typename [:get_entity_type():], TARGET_DATA_TYPE>(m_entity);
		}

	public:
		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set()
		{
			(set_occupation<TARGET_DATA_TYPES>(), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set(TARGET_DATA_TYPES... in_data)
		{
			(set_occupation<TARGET_DATA_TYPES>(), ...);
			(set_data_value<TARGET_DATA_TYPES>(in_data), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset()
		{
			(reset_occupation<TARGET_DATA_TYPES>(), ...);
		}

	private:
		[:get_entity_type():] m_entity;
		[:get_store_type():]& m_store;
	};
}