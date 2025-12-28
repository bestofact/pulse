#pragma once

#include "ecs/concepts/archetypechunkconcept.h"
#include "ecs/concepts/dataconcept.h"

#include "foundation/types.h"
#include "foundation/meta.h"

#include <meta>
#include <array>

namespace pulse::ecs
{
	template<pulse::ecs::concepts::ArchetypeChunk... ARCHETYPE_CHUNK_TYPES>
	struct ArchetypeStore
	{
		struct Store;

#pragma region -------------------- META ----------------------
		
		consteval static pulse::u64 get_entity_capacity()
		{
			constexpr auto info = pulse::meta::get_first_arg<ARCHETYPE_CHUNK_TYPES...>();
			return [:info:]::get_entity_capacity();
		}

		consteval static pulse::u64 get_archetype_chunk_count()
		{
			return pulse::meta::get_arg_count<ARCHETYPE_CHUNK_TYPES...>();
		}

		consteval static std::array<std::meta::info, get_archetype_chunk_count()> get_archetype_chunk_types()
		{
			const std::array<std::meta::info, get_archetype_chunk_count()> types = {
				^^ARCHETYPE_CHUNK_TYPES...
			};
			return types;
		}

		consteval static std::meta::info get_archetype_chunk_type_for_data_type(std::meta::info in_info)
		{
			auto archetypeChunkType = std::meta::info();
			const auto find_archetype_chunk_type = [&]<pulse::ecs::concepts::ArchetypeChunk ARCHETYPE_CHUNK_TYPE>()
			{
				constexpr auto dataTypes = ARCHETYPE_CHUNK_TYPE::get_data_types();
				for(const auto dataType : dataTypes)
				{
					if(std::meta::is_same_type(dataType, in_info))
					{
						archetypeChunkType = ^^ARCHETYPE_CHUNK_TYPE;
					}
				}
			};

			PULSE_FOR_EACH_VARIADIC_ARGUMENT(
				find_archetype_chunk_type.template operator()<ARCHETYPE_CHUNK_TYPES>()
			);

			return archetypeChunkType;
		}

		consteval static std::meta::info get_store_type()
		{
			return ^^Store;
		}

		consteval static std::meta::data_member_options construct_store_member_options(std::meta::info in_archetypeType)
		{
			return std::meta::data_member_options{
				//.name = std::meta::identifier_of(in_archetypeType)
			};
		}

		consteval static std::meta::info construct_store_member(std::meta::info in_archetypeType)
		{
			return std::meta::data_member_spec(
				in_archetypeType,
				construct_store_member_options(in_archetypeType)
			);
		}

		consteval static void define_store_type()
		{
			const std::array<std::meta::info, get_archetype_chunk_count()> members = {
				construct_store_member(^^ARCHETYPE_CHUNK_TYPES)...
			};

			std::meta::define_aggregate(get_store_type(), members);
		}

		consteval
		{
			define_store_type();
		}

		consteval static std::meta::info get_store_member(std::meta::info in_archetypeType)
		{
			constexpr auto storeType = get_store_type();
			constexpr auto ctx = std::meta::access_context::unchecked();
			const auto members = std::meta::nonstatic_data_members_of(storeType, ctx);
			for(const auto member : members)
			{
				const auto type = std::meta::type_of(member);
				if(std::meta::is_same_type(type, in_archetypeType))
				{
					return member;
				}
			}

			return std::meta::info();
		}

#pragma endregion

		template<pulse::ecs::concepts::Data DATA_TYPE>
		const auto& get_archetype_chunk() const
		{
			constexpr auto archetypeChunkType = get_archetype_chunk_type_for_data_type(^^DATA_TYPE);
			constexpr auto member = get_store_member(archetypeChunkType);
			const auto& chunk = m_store.[:member:];
			return chunk;
		}

		template<pulse::ecs::concepts::Data DATA_TYPE>
		auto& get_archetype_chunk_mutable()
		{
			constexpr auto archetypeChunkType = get_archetype_chunk_type_for_data_type(^^DATA_TYPE);
			constexpr auto member = get_store_member(archetypeChunkType);
			auto& chunk = m_store.[:member:];
			return chunk;
		}

	private:
		[:get_store_type():] m_store;
	};
}