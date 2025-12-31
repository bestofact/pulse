#pragma once

#include "ecs/concepts/entityconcept.h"
#include "foundation/limits.h"
#include "foundation/bits/bitmask.h"

namespace pulse::ecs
{
	template<pulse::ecs::concepts::Entity ENTITY_TYPE>
	struct EntityStore
	{
#pragma region -------------------- META ---------------------
		using EntityType = ENTITY_TYPE;

		consteval static std::meta::info get_entity_type()
		{
			return ^^ENTITY_TYPE;
		}

		consteval static pulse::u64 get_capacity()
		{
			return EntityType::get_capacity();
		}

		consteval static pulse::u64 get_status_size()
		{
			return get_capacity() / 64;
		}

#pragma endregion

		pulse::u64 next_available_entity_index()
		{
			return m_status.find_first_zero();
		}

		EntityType new_entity()
		{
			const pulse::u64 index = next_available_entity_index();
			m_status.set(index);
			
			const EntityType entity(index);
			return entity;
		}

		void free_entity(const EntityType in_entity)
		{
			const pulse::u64 index = in_entity.get_index();
			m_status.reset(index);
		}

	private:
		//pulse::u64  m_status[get_status_size()];
		pulse::BitMask<get_capacity()> m_status;
		//std::bitset<get_capacity()> m_status;
	};
}