#pragma once

#include "ecs/concepts/helpers.h"

#include "foundation/types.h"

#include <algorithm>
#include <concepts>
#include <meta>

namespace pulse::ecs::concepts
{
    consteval bool has_one_static_entity_index_member(std::meta::info in_info)
    {
        constexpr auto entityIndexType = ^^pulse::u64;
        constexpr auto ctx = std::meta::access_context::unchecked();

        bool foundOne = false;
        const auto members = std::meta::static_data_members_of(in_info, ctx);
        for(const auto member : members)
        {
            if(std::meta::is_convertible_type(std::meta::type_of(member), entityIndexType))
            {
                if(foundOne)
                {
                    return false;
                };

                foundOne = true;
            }
        }

        return foundOne;
    }

	template<typename T>
    concept Entity =
       pulse::ecs::concepts::RequireEcsAnyParentNamespace<T>
    //&& std::constructible_from<T>
    //&& std::constructible_from<T, u64>
    && has_one_static_entity_index_member(^^T)
    && requires
    {
        { T::get_capacity() } -> std::same_as<u64>;
    }
    && requires (T t) {
        
        { t.get_index() } -> std::convertible_to<u64>;
    }
    && requires (T& t) {
        { t.set_index(u64(0)) } -> std::same_as<void>;
    };
}