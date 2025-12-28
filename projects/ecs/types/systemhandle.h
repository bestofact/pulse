#pragma once

#include "ecs/concepts/systemconcept.h"
#include "ecs/utils/meta.h"
#include "ecs/utils/invokers.h"

#include "foundation/meta.h"

#include <vector>
#include <meta>

namespace pulse::ecs
{
	template<
		pulse::ecs::concepts::System SYSTEM_TYPE,
		SYSTEM_TYPE SYSTEM_INSTANCE>
	struct SystemHandle
	{
#pragma region -------------- META ---------------
		consteval static std::meta::info get_system()
		{
			return std::meta::reflect_function(SYSTEM_INSTANCE);
		}

		consteval static std::meta::info get_system_type()
		{
			return ^^SYSTEM_TYPE;
		}

		consteval static std::meta::info get_return_type()
		{
			return std::meta::return_type_of(get_system_type());
		}

		consteval static std::meta::info get_system_pointer_type()
		{
			return std::meta::add_pointer(get_system_type());
		}

		consteval static [:get_system_pointer_type():] get_system_instance()
		{
			return SYSTEM_INSTANCE;
		}

		consteval static std::vector<std::meta::info> get_constant_required_component_types()
		{
			std::vector<std::meta::info> types;
			const auto params = std::meta::parameters_of(get_system_type());
			for(const auto param : params)
			{
				if(!std::meta::is_const(param))
				{
					continue;
				}

				const auto type = pulse::meta::decay_all(param);
				if(!pulse::ecs::meta::is_component(type))
				{
					continue;
				}

				types.push_back(type);
			}
			return types;
		}

		consteval static std::vector<std::meta::info> get_mutable_required_component_types()
		{
			std::vector<std::meta::info> types;
			const auto params = std::meta::parameters_of(get_system_type());
			for(const auto param : params)
			{
				if(std::meta::is_const(param))
				{
					continue;
				}

				const auto type = pulse::meta::decay_all(param);
				if(!pulse::ecs::meta::is_component(type))
				{
					continue;
				}

				types.push_back(type);
			}
			return types;
		}

		template<typename RETURN_TYPE>
		consteval static std::vector<std::meta::info> get_produced_output_types()
		{
			return {};
		}

		template<pulse::ecs::concepts::OutputHandle RETURN_TYPE>
		consteval static std::vector<std::meta::info> get_produced_output_types()
		{
			std::vector<std::meta::info> types;
			types.append_range(RETURN_TYPE::get_output_types());
			return types;
		}

		consteval static std::vector<std::meta::info> get_produced_output_types()
		{
			constexpr auto type = get_return_type();
			return get_produced_output_types<typename [:type:]>();
		}

		consteval static std::vector<std::meta::info> get_consumed_output_types()
		{
			std::vector<std::meta::info> types;
			const auto params = std::meta::parameters_of(get_system_type());
			for(const auto param : params)
			{
				const auto type = pulse::meta::decay_all(param);
				if(!pulse::ecs::meta::is_output(type))
				{
					continue;
				}

				types.push_back(type);
			}
			return types;
		}
#pragma endregion
	};
}