#pragma once

#include "foundation/types.h"
#include "foundation/limits.h"

#include <meta>

namespace pulse::ecs
{
	template<u64 DESIRED_ENTITY_COUNT>
	struct Entity
	{
        consteval static pulse::u64 get_capacity()
        {
            return (DESIRED_ENTITY_COUNT == 1) ? 1 : (1 << (64 - std::countl_zero(DESIRED_ENTITY_COUNT - 1)));
        }

        constexpr inline static pulse::u64 s_capacity = get_capacity();

        Entity(const pulse::u64 in_index = invalid_index())
        	: m_index(in_index)
    	{	
    	}

    	pulse::u64 get_index() const
    	{
    		return m_index;
    	}

    	void set_index(const pulse::u64 in_index)
    	{
    		m_index = in_index;
    	}

    private:
    	pulse::u64 m_index = invalid_index();
	};
}