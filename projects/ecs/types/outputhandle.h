#pragma once

#include "ecs/concepts/entityconcept.h"
#include "ecs/concepts/outputconcept.h"

#include "foundation/types.h"
#include "foundation/meta.h"

#include <meta>
#include <bitset>

namespace pulse::ecs
{
	template<pulse::ecs::concepts::Output... OUTPUT_TYPES>
	struct OutputHandle
	{
		struct Result;
		
#pragma region ------------------ META ---------------------
		consteval static pulse::u64 get_output_count()
		{
			return pulse::meta::get_arg_count<OUTPUT_TYPES...>();
		}

		consteval static pulse::u64 get_output_index(std::meta::info in_output)
		{
			return pulse::meta::get_arg_index<OUTPUT_TYPES...>(in_output);
		}

		consteval static std::meta::info get_nth_output_type(pulse::u64 in_n)
		{
			return pulse::meta::get_nth_arg<OUTPUT_TYPES...>(in_n);
		}

		consteval static std::array<std::meta::info, get_output_count()> get_output_types()
		{
			const std::array<std::meta::info, get_output_count()> types = {
				^^OUTPUT_TYPES...
			};
			return types;
		}

		consteval static std::meta::info get_result_type()
		{
			return ^^Result;
		}

		consteval static std::meta::data_member_options construct_output_member_options(std::meta::info in_output)
		{
			return std::meta::data_member_options{
				.name = std::meta::identifier_of(in_output)
			};
		}

		consteval static std::meta::info construct_output_member(std::meta::info in_output)
		{
			return std::meta::data_member_spec(
				in_output,
				construct_output_member_options(in_output)
			);
		}

		consteval static void define_result_type()
		{
			const std::array<std::meta::info, get_output_count()> members = {
				construct_output_member(^^OUTPUT_TYPES)...
			};
			std::meta::define_aggregate(get_result_type(), members);
		}

		consteval static std::meta::info get_status_type()
		{
			return ^^std::bitset<get_output_count()>;
		}

		consteval
		{
			define_result_type();
		}

		consteval static std::meta::info get_status_member(std::meta::info in_output)
		{
			constexpr auto status = get_status_type();
			constexpr auto ctx = std::meta::access_context::unchecked();

			const auto index = get_output_index(in_output);
			return pulse::meta::nth_non_static_data_member_of(status, index);
		}

		consteval static std::meta::info get_result_member(std::meta::info in_output)
		{
			constexpr auto result = get_result_type();
			constexpr auto ctx = std::meta::access_context::unchecked();

			const auto index = get_output_index(in_output);
			return pulse::meta::nth_non_static_data_member_of(result, index);
		}
#pragma endregion

		OutputHandle()
		{
			m_status.reset();
		}

		template<pulse::ecs::concepts::Output OUTPUT_TYPE>
		bool has_result() const
		{
			constexpr auto index = get_output_index(^^OUTPUT_TYPE);
			return m_status.test(index);
		}

		template<pulse::ecs::concepts::Output OUTPUT_TYPE>
		const OUTPUT_TYPE& get_result() const
		{
			constexpr auto result = get_result_member(^^OUTPUT_TYPE);
			return m_result.[:result:];
		}

		template<pulse::ecs::concepts::Output OUTPUT_TYPE>
		void set_result(const OUTPUT_TYPE& in_result)
		{
			constexpr auto index = get_output_index(^^OUTPUT_TYPE);
			m_status.set(index);

			constexpr auto result = get_result_member(^^OUTPUT_TYPE);
			m_result.[:result:] = in_result;
		}

		template<pulse::ecs::concepts::Output OUTPUT_TYPE>
		void reset_result()
		{
			constexpr auto index = get_output_index(^^OUTPUT_TYPE);
			m_status.reset(index);
		}

	private:
		[:get_status_type():] m_status;
		[:get_result_type():] m_result;
	};
}