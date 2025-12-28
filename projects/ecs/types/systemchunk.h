#pragma once

#include "ecs/concepts/systemhandleconcept.h"

#include "foundation/types.h"
#include "foundation/meta.h"

#include <meta>
#include <array>

namespace pulse::ecs
{
	template<pulse::ecs::concepts::SystemHandle... SYSTEM_HANDLE_TYPES>
	struct SystemChunk
	{
		struct Chunk;
#pragma region --------------- META ----------------
		consteval static pulse::u64 get_system_handle_count()
		{
			return pulse::meta::get_arg_count<SYSTEM_HANDLE_TYPES...>();
		}

		consteval static pulse::u64 get_system_handle_index(std::meta::info in_info)
		{
			return pulse::meta::get_arg_index<SYSTEM_HANDLE_TYPES...>(in_info);
		}

		consteval static std::meta::info get_nth_system_handle_type(const pulse::u64 in_n)
		{
			return pulse::meta::get_nth_arg<SYSTEM_HANDLE_TYPES...>(in_n);
		}

		consteval static std::array<std::meta::info, get_system_handle_count()> get_system_handle_types()
		{
			const std::array<std::meta::info, get_system_handle_count()> types = {
				^^SYSTEM_HANDLE_TYPES...
			};
			return types;
		}
#pragma endregion

	};
}