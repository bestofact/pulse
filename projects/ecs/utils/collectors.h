#pragma once

#include "ecs/concepts/systemhandleconcept.h"
#include "ecs/utils/meta.h"
#include "foundation/meta.h"

#include <algorithm>
#include <meta>
#include <vector>

namespace pulse::ecs::collectors
{
	consteval std::vector<std::meta::info> collect_component_types(std::meta::info in_namespace)
	{
		std::vector<std::meta::info> types;

		if(pulse::ecs::meta::is_entity_namespace(in_namespace))
		{
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::members_of(in_namespace, ctx);
			for(const auto member : members)
			{
				if(pulse::ecs::meta::is_component(member))
				{
					types.push_back(member);
				}
			}
		}

		return types;
	}

	consteval std::vector<std::meta::info> collect_output_types(std::meta::info in_namespace)
	{
		std::vector<std::meta::info> types;

		if(pulse::ecs::meta::is_entity_namespace(in_namespace))
		{
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::members_of(in_namespace, ctx);
			for(const auto member : members)
			{
				if(pulse::ecs::meta::is_output(member))
				{
					types.push_back(member);
				}
			}
		}

		return types;
	}

	consteval std::vector<std::meta::info> collect_systems(std::meta::info in_namespace)
	{
		std::vector<std::meta::info> types;

		if(pulse::ecs::meta::is_entity_namespace(in_namespace))
		{
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::members_of(in_namespace, ctx);
			for(const auto member : members)
			{
				if(pulse::ecs::meta::is_system(member))
				{
					types.push_back(member);
				}
			}
		}

		return types;
	}

	consteval std::vector<std::vector<std::meta::info>> topological_sort_system_handles(std::vector<std::meta::info> in_handles)
	{
		struct Dependency
		{
			std::meta::info m_consumer = std::meta::info();
			std::vector<std::meta::info> m_producers;
		};

		// construct system dependencies.
		std::vector<Dependency> dependencies;
		for(const auto consumer : in_handles)
		{
			Dependency dependency;
			dependency.m_consumer = consumer;

			for(const auto producer : in_handles)
			{
				if(pulse::ecs::meta::has_dependency(producer, consumer))
				{
					dependency.m_producers.push_back(producer);
				}
			}

			dependencies.push_back(dependency);
		}

		// some utilities.
		const auto find_consumers_without_producers = [&dependencies]()
		{
			std::vector<std::meta::info> types;
			for(const auto& dependency : dependencies)
			{
				if(dependency.m_producers.empty())
				{
					types.push_back(dependency.m_consumer);
				}
			}
			return types;
		};

		const auto erase_producers = [&dependencies](std::vector<std::meta::info> in_info)
		{
			for(const auto info : in_info)
			{
				for(auto& dependency : dependencies)
				{
					std::erase(dependency.m_producers, info);
				}

				std::erase_if(
					dependencies, 
					[info](const Dependency& in_dependency) 
					{
						return in_dependency.m_consumer == info;
					}
				);
			}
		};

		// sort dependencies into layers. systems in the same layer does not have a dependency to each other.
		std::vector<std::vector<std::meta::info>> layers;
		while(!dependencies.empty())
		{
			// consumers without producers, now producers in this iteration.
			const auto producers = find_consumers_without_producers();
			if(producers.empty())
			{
				// todo(anilt) : cyclic dependency here. somehow assert in comptime.
				break;
			}

			layers.push_back(producers);
			erase_producers(producers);
		}

		return layers;
	}
}












