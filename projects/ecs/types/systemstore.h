#pragma once

#include "ecs/concepts/systemchunkconcept.h"

#include "foundation/types.h"
#include "foundation/meta.h"

#include <meta>
#include <array>

namespace pulse::ecs
{
	template<pulse::ecs::concepts::SystemChunk... SYSTEM_CHUNK_TYPES>
	struct SystemStore
	{

#pragma region ------------------ META ---------------------
		consteval static pulse::u64 get_system_chunk_count()
		{
			return pulse::meta::get_arg_count<SYSTEM_CHUNK_TYPES...>();
		}

		consteval static pulse::u64 get_system_chunk_index(std::meta::info in_info)
		{
			return pulse::meta::get_arg_index<SYSTEM_CHUNK_TYPES...>(in_info);
		}

		consteval static std::meta::info get_nth_system_chunk_type(const pulse::u64 in_n)
		{
			return pulse::meta::get_nth_arg<SYSTEM_CHUNK_TYPES...>(in_n);
		}

		consteval static std::array<std::meta::info, get_system_chunk_count()> get_system_chunk_types()
		{
			const std::array<std::meta::info, get_system_chunk_count()> types = {
				^^SYSTEM_CHUNK_TYPES...
			};
			return types;
		}

#pragma endregion

	};
}