#pragma once

#include "ecs/types/world.h"
#include "ecs/utils/collectors.h"
#include "ecs/utils/meta.h"

#include "ecs/types/archetypechunk.h"
#include "ecs/types/archetypestore.h"
#include "ecs/types/entitystore.h"
#include "ecs/types/systemhandle.h"
#include "ecs/types/systemchunk.h"
#include "ecs/types/systemstore.h"

#include <meta>


namespace pulse::ecs::generators
{
namespace __detail
{
	consteval std::meta::info generate_simple_entity_store(std::meta::info in_entity)
	{
		constexpr auto storeTemplate = ^^pulse::ecs::EntityStore;
		const auto store = std::meta::substitute(storeTemplate, { in_entity });
		return store;
	}

	consteval std::meta::info generate_sparse_archetype_store(std::vector<std::meta::info> in_types)
	{
		constexpr auto archetypeTemplate = ^^pulse::ecs::ArchetypeChunk;
		constexpr auto storeTemplate = ^^pulse::ecs::ArchetypeStore;
		
		std::vector<std::meta::info> archetypes;
		for(const auto type : in_types)
		{
			const auto archetype = std::meta::substitute(archetypeTemplate, { type });
			archetypes.push_back(archetype);
		}

		const auto store = std::meta::substitute(storeTemplate, archetypes);
		return store;
	}

	consteval std::meta::info generate_layered_system_store(std::vector<std::meta::info> in_systems)
	{
		constexpr auto handleTemplate = ^^pulse::ecs::SystemHandle;
		constexpr auto chunkTemplate = ^^pulse::ecs::SystemChunk;
		constexpr auto storeTemplate = ^^pulse::ecs::SystemStore;

		std::vector<std::meta::info> handles;
		for(const auto system : in_systems)
		{
			const auto handle = std::meta::substitute(handleTemplate, { std::meta::type_of(system), system });
			handles.push_back(handle);
		}

		std::vector<std::meta::info> chunks;
		const auto layers = pulse::ecs::collectors::topological_sort_system_handles(handles);
		for(const auto& layer : layers)
		{
			const auto chunk = std::meta::substitute(chunkTemplate, layer);
			chunks.push_back(chunk);
		}

		const auto store = std::meta::substitute(storeTemplate, chunks);
		return store;
	}
}

	consteval std::meta::info generate_default_entity_store(std::meta::info in_namespace)
	{
		const auto entity = pulse::ecs::meta::get_entity_type(in_namespace);
		return __detail::generate_simple_entity_store(entity);
	}

	consteval std::meta::info generate_default_component_store(std::meta::info in_namespace)
	{
		const auto components = pulse::ecs::collectors::collect_component_types(in_namespace);
		return __detail::generate_sparse_archetype_store(components);
	}

	consteval std::meta::info generate_default_output_store(std::meta::info in_namespace)
	{
		const auto outputs = pulse::ecs::collectors::collect_output_types(in_namespace);
		return __detail::generate_sparse_archetype_store(outputs);
	}

	consteval std::meta::info generate_default_system_store(std::meta::info in_namespace)
	{
		const auto systems = pulse::ecs::collectors::collect_systems(in_namespace);
		return __detail::generate_layered_system_store(systems);
	}

	consteval std::meta::info generate_default_world(std::meta::info in_namespace)
	{
		const auto world = std::meta::substitute(
			^^pulse::ecs::World,
			{
				generate_default_entity_store(in_namespace),
				generate_default_component_store(in_namespace),
				generate_default_output_store(in_namespace),
				generate_default_system_store(in_namespace)
			}
		);
		return world;
	}
}