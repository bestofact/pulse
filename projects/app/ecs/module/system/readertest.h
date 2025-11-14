#pragma once

#include "app/ecs/module/component/name.h"
#include <iostream>
#include <random>

inline std::string random_string(std::size_t length, size_t seed)
	{
	    static const std::string chars =
	        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	    std::mt19937 gen(static_cast<unsigned>(seed));
	    std::uniform_int_distribution<> dist(0, static_cast<int>(chars.size() - 1));

	    std::string result;
	    result.reserve(length);

	    for (std::size_t i = 0; i < length; ++i)
	        result.push_back(chars[dist(gen)]);

	    return result;
	}

	inline bool random_bool(double chance)
	{
	    static std::random_device rd;
	    static std::mt19937 gen(rd());
	    std::bernoulli_distribution dist(chance);
	    return dist(gen);
	}

namespace pulse::ecs::module
{
	inline void ReadTest(
		Name& in_name,
		const Fame& in_fame)
	{
		in_name.m_name = random_string(10, in_fame.m_lvl);
		std::cout<< "Read Result" << in_name.m_name << std::endl;
	}

	inline void Readily(Fame& in_fame)
	{
		in_fame.m_lvl++;
		std::cout<< "Fame Increased" << in_fame.m_lvl << std::endl;
	}
}