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
		void set_data_value(const [:get_entity_type():] in_entity, const TARGET_DATA_TYPE& in_data)
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			auto& data = chunk.template get_data_mutable<typename [:get_entity_type():], TARGET_DATA_TYPE>(in_entity);
			data = in_data;
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		const TARGET_DATA_TYPE& get_data_value(const [:get_entity_type():] in_entity) const
		{
			const auto& chunk = m_store.template get_archetype_chunk<TARGET_DATA_TYPE>();
			const auto& data = chunk.template get_data<typename [:get_entity_type():], TARGET_DATA_TYPE>(in_entity);
			return data;
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		TARGET_DATA_TYPE& get_data_value_mutable(const [:get_entity_type():] in_entity)
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			auto& data = chunk.template get_data_mutable<typename [:get_entity_type():], TARGET_DATA_TYPE>(in_entity);
			return data;
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		void set_occupation(const [:get_entity_type():] in_entity)
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			chunk.template set_occupation<typename [:get_entity_type():], TARGET_DATA_TYPE>(in_entity);
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		void reset_occupation(const [:get_entity_type():] in_entity)
		{
			auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			chunk.template reset_occupation<typename [:get_entity_type():], TARGET_DATA_TYPE>(in_entity);
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		void test_occupation(const [:get_entity_type():] in_entity) const
		{
			const auto& chunk = m_store.template get_archetype_chunk_mutable<TARGET_DATA_TYPE>();
			return chunk.template test_occupation<typename [:get_entity_type():], TARGET_DATA_TYPE>(in_entity);
		}

	public:

		const [:get_entity_type():] me() const
		{
			return m_entity;
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set()
		{
			(set_occupation<TARGET_DATA_TYPES>(m_entity), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set(TARGET_DATA_TYPES... in_data)
		{
			(set_occupation<TARGET_DATA_TYPES>(m_entity), ...);
			(set_data_value<TARGET_DATA_TYPES>(m_entity, in_data), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset()
		{
			(reset_occupation<TARGET_DATA_TYPES>(m_entity), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set(const [:get_entity_type():] in_entity)
		{
			(set_occupation<TARGET_DATA_TYPES>(in_entity), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set(const [:get_entity_type():] in_entity, TARGET_DATA_TYPES... in_data)
		{
			(set_occupation<TARGET_DATA_TYPES>(in_entity), ...);
			(set_data_value<TARGET_DATA_TYPES>(in_entity, in_data), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset(const [:get_entity_type():] in_entity)
		{
			(reset_occupation<TARGET_DATA_TYPES>(in_entity), ...);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void test(const [:get_entity_type():] in_entity) const
		{
			return (test_occupation<TARGET_DATA_TYPES>(in_entity) && ...);
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		const TARGET_DATA_TYPE& value(const [:get_entity_type():] in_entity) const
		{
			return get_data_value<TARGET_DATA_TYPE>(in_entity);
		}

		template<pulse::ecs::concepts::Data TARGET_DATA_TYPE>
		TARGET_DATA_TYPE& value_mutable(const [:get_entity_type():] in_entity)
		{
			return get_data_value_mutable<TARGET_DATA_TYPE>(in_entity);
		}

	private:
		[:get_entity_type():] m_entity;
		[:get_store_type():]& m_store;
	};
}