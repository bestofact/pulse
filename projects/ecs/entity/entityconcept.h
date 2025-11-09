#pragma once

#include <bitset>
#include <concepts>
#include <cstddef>
#include <meta>

namespace pulse::ecs
{
	template <typename T>
	concept EntityConcept = requires (T t)
    {
        { t.m_component_bitset.size() } -> std::convertible_to<size_t>;
        std::same_as<decltype(t.m_component_bitset), std::bitset<t.m_component_bitset.size()>>;
    };
}