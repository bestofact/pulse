#pragma once

#include <bit>
#include <cstddef>

#include "ecs/v3/entity/entitycapacity.h"
#include "ecs/v3/concepts/entitycapacityconcept.h"

namespace pulse::ecs
{
    template<
        pulse::ecs::concepts::EntityCapacity CAPACITY_TYPE,
        CAPACITY_TYPE DESIRED_ENTITY_COUNT>
    struct Entity
    {
        using CapacityType = CAPACITY_TYPE;

        consteval static CapacityType calculate_capacity()
        {
            return (DESIRED_ENTITY_COUNT == 1) ? 1 : (1 << (64 - std::countl_zero(DESIRED_ENTITY_COUNT - 1)));
        }
        static inline constexpr CapacityType s_capacity = calculate_capacity();

        consteval std::size_t invalid_index()
        {
            return static_cast<std::size_t>(-1);
        }

        bool is_valid() const
        {
            return get_index() != invalid_index();
        }

        std::size_t get_index() const
        {
            return m_index;
        }

        void set_index(const std::size_t in_index)
        {
            m_index = in_index;
        }

private:
        std::size_t m_index = invalid_index();
    };
}
