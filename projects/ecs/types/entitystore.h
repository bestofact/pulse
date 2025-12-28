#pragma once

#include "ecs/concepts/entityconcept.h"
#include "foundation/limits.h"

#include <bitset>

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

#pragma endregion

		pulse::u64 next_available_entity_index()
		{
			for(pulse::u64 index = 0; index < get_capacity(); ++index)
			{
				if(!m_status.test(index))
				{
					return index;
				}
			}

			return pulse::invalid_index();
		}

		EntityType new_entity()
		{
			const pulse::u64 index = next_available_entity_index();
			const EntityType entity(index);
			return entity;
		}

		void free_entity(const EntityType in_entity)
		{
			const pulse::u64 index = in_entity.get_index();
			m_status.reset(index);
		}

	private:
		std::bitset<get_capacity()> m_status;
	};
}