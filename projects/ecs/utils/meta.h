#pragma once

#include "ecs/concepts/componentconcept.h"
#include "ecs/concepts/dataconcept.h"
#include "ecs/concepts/entityconcept.h"
#include "ecs/concepts/hasdependencyconcept.h"
#include "ecs/concepts/outputconcept.h"
#include "ecs/concepts/outputhandleconcept.h"
#include "ecs/concepts/systemconcept.h"
#include "ecs/concepts/systemhandleconcept.h"

#include "foundation/meta.h"
#include "foundation/types.h"
#include "foundation/limits.h"

#include <meta>

namespace pulse::ecs::meta
{
	constexpr static u64 invalid_index()
    {
    	return pulse::max<u64>();
    }

    consteval std::meta::info get_ecs_namespace()
    {
    	return ^^pulse::ecs;
    }

	consteval std::meta::info get_component_concept()
	{
		return ^^pulse::ecs::concepts::Component;
	}

	consteval std::meta::info get_data_concept()
	{
		return ^^pulse::ecs::concepts::Data;
	}

	consteval std::meta::info get_entity_concept()
	{
		return ^^pulse::ecs::concepts::Entity;
	}

	consteval std::meta::info get_output_concept()
	{
		return ^^pulse::ecs::concepts::Output;
	}

	consteval std::meta::info get_output_wrapper_concept()
	{
		return ^^pulse::ecs::concepts::OutputHandle;
	}

	consteval std::meta::info get_system_concept()
	{
		return ^^pulse::ecs::concepts::System;
	}

	consteval std::meta::info get_system_handle_concept()
	{
		return ^^pulse::ecs::concepts::SystemHandle;
	}

	consteval std::meta::info get_has_dependency_concept()
	{
		return ^^pulse::ecs::concepts::HasDependency;
	}

	consteval bool is_component(std::meta::info in_info)
	{
		return std::meta::is_type(in_info) && pulse::meta::is_concept_satisfied(in_info, get_component_concept());
	}

	consteval bool is_data(std::meta::info in_info)
	{
		return std::meta::is_type(in_info) && pulse::meta::is_concept_satisfied(in_info, get_data_concept());
	}

	consteval bool is_entity(std::meta::info in_info)
	{
		return std::meta::is_type(in_info) && pulse::meta::is_concept_satisfied(in_info, get_entity_concept());
	}

	consteval bool is_output(std::meta::info in_info)
	{
		return std::meta::is_type(in_info) && pulse::meta::is_concept_satisfied(in_info, get_output_concept());
	}

	consteval bool is_output_wrapper(std::meta::info in_info)
	{
		return std::meta::is_type(in_info) && pulse::meta::is_concept_satisfied(in_info, get_output_wrapper_concept());
	}

	consteval bool is_system(std::meta::info in_info)
	{
		const auto type = std::meta::is_function(in_info) ? std::meta::type_of(in_info) : in_info;
		return std::meta::is_type(type) && pulse::meta::is_concept_satisfied(type, get_system_concept());
	}

	consteval bool is_system_handle(std::meta::info in_info)
	{
		return std::meta::is_type(in_info) && pulse::meta::is_concept_satisfied(in_info, get_system_handle_concept());
	}

	consteval bool has_dependency(std::meta::info in_producer, std::meta::info in_consumer)
	{
		return std::meta::is_type(in_producer) && std::meta::is_type(in_consumer) && pulse::meta::is_concept_satisfied({in_producer, in_consumer}, get_has_dependency_concept());
	}

	consteval bool is_entity_capacity_member(std::meta::info in_info)
	{
		return std::meta::is_static_member(in_info)
			&& std::meta::is_convertible_type(std::meta::type_of(in_info), ^^pulse::u64);
	}

	consteval bool is_entity_namespace(std::meta::info in_info)
	{
		if(!std::meta::is_namespace(in_info))
		{
			return false;
		}

		if(!pulse::meta::is_child_of(in_info, get_ecs_namespace(), false, 0))
		{
			return false;
		}

		if(!pulse::meta::any_member_satisfies_concept(in_info, get_entity_concept(), true, 0))
		{
			return false;
		}

		return true;
	}

	consteval std::meta::info get_entity_namespace(std::meta::info in_info)
	{
		if(is_entity_namespace(in_info))
		{
			return in_info;
		}

		if(is_component(in_info) || is_output(in_info) || is_system(in_info))
		{
			return std::meta::parent_of(in_info);
		}

		if(pulse::meta::can_parent_of(in_info))
		{
			return get_entity_namespace(std::meta::parent_of(in_info));
		}

		return std::meta::info();
	}

	consteval std::meta::info get_entity_type(std::meta::info in_info)
	{
		constexpr auto ctx = std::meta::access_context::unchecked();
		const auto entityNamespace = get_entity_namespace(in_info);
		const auto members = std::meta::members_of(entityNamespace, ctx);
		for(const auto member : members)
		{
			if(is_entity(member))
			{
				return member;
			}
		}

		return std::meta::info();
	}

	consteval std::meta::info get_entity_capacity_member(std::meta::info in_info)
	{
		if(is_entity(in_info))
		{
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::static_data_members_of(in_info, ctx);
			for(const auto member : members)
			{
				if(is_entity_capacity_member(member))
				{
					return member;
				}
			}

			return std::meta::info();
		}

		return get_entity_capacity_member(get_entity_type(in_info));
	}

	consteval u64 get_entity_capacity(std::meta::info in_info)
	{
		const auto member = get_entity_capacity_member(in_info);
		const auto value = std::meta::extract<u64>(member);
		return value;
	}
}