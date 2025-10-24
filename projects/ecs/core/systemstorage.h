#pragma once
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <meta>
#include <type_traits>
#include <variant>

namespace pulse::ecs
{
	template<typename _system_type, _system_type _system_instance>
	struct SystemPointer
	{
		using SystemFunctionPointerType = std::add_pointer_t<_system_type>;
		SystemFunctionPointerType m_pointer = _system_instance;
	};

	template<typename _system_pointer_type, std::ptrdiff_t _system_pointer_member_offset>
	struct SystemExecutionInfo
	{
		using SystemPointerType = _system_pointer_type;
		using SystemFunctionPointerType = SystemPointerType::SystemFunctionPointerType;
		using SystemType = std::remove_pointer_t<SystemFunctionPointerType>;
		static constexpr inline std::ptrdiff_t s_system_pointer_member_offset = _system_pointer_member_offset;
	};

	template<typename _system_storage_type, typename _system_exection_info>
	void execute_system(const _system_storage_type& in_system_storage)
	{
		char* system_storage_char_ptr = reinterpret_cast<char*>(&const_cast<_system_storage_type&>(in_system_storage));
		auto* system_pointer = reinterpret_cast<_system_exection_info::SystemPointerType*>(system_storage_char_ptr + _system_exection_info::s_system_pointer_member_offset);
		system_pointer->m_pointer();
	}

	template<typename _system_storage_type, typename _system_exection_info>
	void debug_systems(const _system_storage_type& in_system_storage)
	{
		std::cout << "Debug System : " << _system_exection_info::s_system_pointer_member_offset << std::endl;
	}

	template<typename _system_storage_type, typename ... _system_execution_info_types>
	struct SystemExecutorSequential
	{
		void execute_systems(const _system_storage_type& in_system_storage)
		{
			(pulse::ecs::execute_system<_system_storage_type, _system_execution_info_types>(in_system_storage), ...);
		}

		void debug_systems(const _system_storage_type& in_system_storage)
		{
			(pulse::ecs::debug_systems<_system_storage_type, _system_execution_info_types>(in_system_storage), ...);
		}
	};

	consteval bool is_system(std::meta::info in_info)
	{
		return std::meta::is_function(in_info);
	}

	template<std::meta::info _entity_namespace_info, typename _system_storage_type>
	consteval void define_system_storage()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();
		std::vector<std::meta::info> system_storage_member_infos;

		auto entity_namespace_member_infos = std::meta::members_of(_entity_namespace_info, ctx);
		for(auto member_info : entity_namespace_member_infos)
		{
			if(is_system(member_info))
			{
				auto system_type = std::meta::type_of(member_info);
				auto system_storage_member_info = std::meta::data_member_spec(
					std::meta::substitute(^^SystemPointer, { system_type, member_info }),
					{
						.name = std::meta::identifier_of(member_info)
					}
				);

				system_storage_member_infos.push_back(system_storage_member_info);
			}
		}

		std::meta::define_aggregate(^^_system_storage_type, system_storage_member_infos);
	}

	template<std::meta::info _system_storage_type_info, typename _system_executor_sequential_type>
	consteval void define_system_executor_sequential()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();

		std::vector<std::meta::info> system_executor_template_argument_infos;
		system_executor_template_argument_infos.push_back(_system_storage_type_info);

		auto system_storage_member_infos = std::meta::members_of(_system_storage_type_info, ctx);
		for(auto member_info : system_storage_member_infos)
		{
			if(std::meta::is_nonstatic_data_member(member_info))
			{
				auto member_type_info = std::meta::type_of(member_info);
				auto member_offset = std::meta::offset_of(member_info).bytes;
				auto member_offset_info = std::meta::reflect_constant(member_offset);
				auto system_execution_info_type_info = std::meta::substitute(^^SystemExecutionInfo, { member_type_info, member_offset_info });
				system_executor_template_argument_infos.push_back(system_execution_info_type_info);
			}
		}

		auto system_executor_type_info = std::meta::substitute(^^SystemExecutorSequential, system_executor_template_argument_infos);
		auto system_executor_data_member_spec_info = std::meta::data_member_spec(system_executor_type_info, { .name = "m_system_executor"});
		std::meta::define_aggregate(^^_system_executor_sequential_type, {system_executor_data_member_spec_info});
	}
}







