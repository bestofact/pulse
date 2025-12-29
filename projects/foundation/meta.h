#pragma once

#include "foundation/types.h"
#include "foundation/limits.h"

#include <concepts>
#include <meta>
#include <array>

#define PULSE_FOR_EACH_VARIADIC_ARGUMENT(Body)\
    ((Body), ...)

namespace pulse::meta
{
	template<typename... Ts>
	consteval pulse::u64 get_arg_count()
	{
		return sizeof...(Ts);
	}

	template<typename... Ts>
	consteval std::array<std::meta::info, get_arg_count<Ts...>()> get_arg_types()
	{
		const std::array<std::meta::info, get_arg_count<Ts...>()> types = {
			^^Ts...
		};
		return types;
	}

	template<typename... Ts>
	consteval std::meta::info get_nth_arg(const pulse::u64 in_n)
	{
		const std::array<std::meta::info, get_arg_count<Ts...>()> infos = {
			^^Ts...
		};
		return infos[in_n];
	}

	template<typename... Ts>
	consteval std::meta::info get_first_arg()
	{
		return get_nth_arg<Ts...>(0);
	}

	template<typename... Ts>
	consteval std::meta::info get_last_arg()
	{
		return get_nth_arg<Ts...>(get_arg_count<Ts...>() - 1);
	}

	template<typename... Ts>
	consteval pulse::u64 get_arg_index(std::meta::info in_arg)
	{
		for(pulse::u64 index = 0; index < get_arg_count<Ts...>(); ++index)
		{
			if(get_nth_arg<Ts...>(index) == in_arg)
			{
				return index;
			}
		}
		return invalid_index();
	}

	consteval std::meta::info get_global_namespace()
	{
		return ^^::;
	}

	consteval bool can_members_of(std::meta::info in_info)
	{
	    return std::meta::is_namespace(in_info)
	    	|| (std::meta::is_type(in_info) && (std::meta::is_class_type(in_info) || std::meta::is_union_type(in_info)));
	}

	consteval bool can_parent_of(std::meta::info in_info)
	{
    	return in_info != get_global_namespace();
	}

	template<std::meta::reflection_range R = std::initializer_list<std::meta::info>>
	consteval bool is_concept_satisfied(R&& in_types, std::meta::info in_concept)
	{
		const auto substituteInfo = std::meta::substitute(in_concept, in_types);
		return substituteInfo != std::meta::info()
			?  std::meta::extract<bool>(substituteInfo)
			:  false;
	}

	consteval bool is_concept_satisfied(std::meta::info in_type, std::meta::info in_concept)
	{
		const auto substituteInfo = std::meta::substitute(in_concept, {in_type});
		return substituteInfo != std::meta::info()
			?  std::meta::extract<bool>(substituteInfo)
			:  false;
	}

	template<std::meta::reflection_range RANGE_TYPE>
	consteval bool is_concept_satisfied_for_all(RANGE_TYPE in_types, std::meta::info in_concept)
	{
		for(const auto type : in_types)
		{
			if(!is_concept_satisfied(type, in_concept))
			{
				return false;
			}
		}
		return true;
	}

	template<typename F>
	requires requires(F&& f) {
        { f(std::meta::info()) } -> std::same_as<bool>;
    }
	consteval std::meta::info find_member_if(std::meta::info in_parent, bool in_dealias, u64 in_level, F in_func)
	{
		constexpr auto invalid_level = pulse::max<decltype(in_level)>();
		constexpr auto ctx = std::meta::access_context::unchecked();
		const auto members = std::meta::members_of(in_parent, ctx);
		for(const auto member : members)
		{
			if(in_level == invalid_level || in_level == 0)
			{
				if(in_func(member))
				{
					return member;
				}

				if(in_dealias && std::meta::is_type_alias(member) && in_func(std::meta::dealias(member)))
				{
					return member;
				}
			}
			
			if(in_level != 0 && can_members_of(member))
			{
				const decltype(in_level) step = in_level == invalid_level ? 0 : 1;
				const auto recursedMember = find_member_if(member, in_dealias, in_level - step, in_func);
				if(recursedMember != std::meta::info())
				{
				 	return recursedMember;
				}
			}
		}

		return std::meta::info();
	}

	template<typename F>
	requires requires(F&& f) {
        { f(std::meta::info()) } -> std::same_as<bool>;
    }
	consteval bool contains_member_if(std::meta::info in_parent, bool in_dealias, u64 in_level, F in_func)
	{
		return find_member_if(in_parent, in_dealias, in_level, in_func) != std::meta::info();
	}

	consteval bool is_child_of(std::meta::info in_child, std::meta::info in_parent, bool in_dealias, u64 in_level = pulse::max<u64>())
	{
		const auto pred = [in_child](std::meta::info in_info) consteval
		{
			return in_child == in_info;
		};
		return contains_member_if(in_parent, in_dealias, in_level, pred);
	}

	consteval bool any_member_satisfies_concept(std::meta::info in_parent, std::meta::info in_concept, bool in_dealias, u64 in_level = pulse::max<u64>())
	{
		const auto pred = [in_concept](std::meta::info in_info) consteval
		{
			return std::meta::is_type(in_info) && is_concept_satisfied(in_info, in_concept);
		};
		return contains_member_if(in_parent, in_dealias, in_level, pred);
	}

	consteval bool any_neioughbour_satisfies_concept(std::meta::info in_target, std::meta::info in_concept, bool in_dealias)
	{
		if(!can_parent_of(in_target))
		{
			return false;
		}

		constexpr auto ctx = std::meta::access_context::unchecked();
		const auto parent = std::meta::parent_of(in_target);
		return any_member_satisfies_concept(parent, in_concept, in_dealias, 0);
	}

	consteval std::meta::info decay_all(std::meta::info in_info)
	{
		if(std::meta::is_pointer_type(in_info))
		{
			return decay_all(std::meta::remove_pointer(in_info));
		}

		const auto decayed = std::meta::decay(in_info);
		if(decayed != in_info)
		{
			return decay_all(decayed);
		}

		return decayed;
	}

	consteval std::meta::info nth_non_static_data_member_of(std::meta::info in_type, pulse::u64 in_n)
	{
		constexpr auto ctx = std::meta::access_context::unchecked();
		const auto member = std::meta::nonstatic_data_members_of(in_type, ctx)[in_n];
		return member;
	}
}





