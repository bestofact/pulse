#pragma once

#include "ecs/concepts/dataconcept.h"

#include "foundation/meta.h"
#include <meta>


namespace pulse::ecs
{
	template<typename TYPE>
	requires ( pulse::ecs::concepts::Data<typename [:pulse::meta::decay_all(^^TYPE):]>)
	struct DataAccess
	{
#pragma region ------------------ META -------------------

		consteval static std::meta::info get_type()
		{
			return ^^TYPE;
		}

		consteval static std::meta::info get_data_type()
		{
			return pulse::meta::decay_all(get_type());
		}

		consteval static bool is_mutable()
		{
			return !std::meta::is_const_type(std::meta::remove_reference(get_type()));
		}

		consteval static bool is_optional()
		{
			return false;
		}
#pragma endregion
	};
}