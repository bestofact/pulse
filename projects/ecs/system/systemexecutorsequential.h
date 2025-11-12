#pragma once
#include "ecs/system/systemexecutioninfo.h"
#include <meta>

namespace pulse::ecs
{
	template<typename _system_storage_type, typename _component_storage_type, typename ... _system_execution_info_types>
	struct SystemExecutorSequential
	{
		void execute_systems(
			const pulse::ecs::Entity in_entity,
			const _system_storage_type& in_system_storage, 
			const _component_storage_type& in_component_storage)
		{
			(_system_execution_info_types::execute(in_entity, in_system_storage, in_component_storage), ...);
		}

		void debug_systems(
			const pulse::ecs::Entity in_entity,
			const _system_storage_type& in_system_storage,
			const _component_storage_type& in_component_storage)
		{
			(_system_execution_info_types::debug(in_entity, in_system_storage, in_component_storage), ...);
		}
	};

	template<std::meta::info _system_storage_type_info, std::meta::info _component_storage_type_info, typename _system_executor_sequential_type>
	consteval void define_system_executor_sequential()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();

		std::vector<std::meta::info> system_executor_template_argument_infos;
		system_executor_template_argument_infos.push_back(_system_storage_type_info);
		system_executor_template_argument_infos.push_back(_component_storage_type_info);

		auto system_storage_member_infos = std::meta::nonstatic_data_members_of(_system_storage_type_info, ctx);
		for(auto member_info : system_storage_member_infos)
		{
			auto member_type_info = std::meta::type_of(member_info);
			auto member_offset = std::meta::offset_of(member_info).bytes;
			auto member_offset_info = std::meta::reflect_constant(member_offset);
			auto system_execution_info_type_info = std::meta::substitute(^^SystemExecutor, { _system_storage_type_info, _component_storage_type_info, member_type_info, member_offset_info });
			system_executor_template_argument_infos.push_back(system_execution_info_type_info);		
		}

		auto system_executor_type_info = std::meta::substitute(^^SystemExecutorSequential, system_executor_template_argument_infos);
		auto system_executor_data_member_spec_info = std::meta::data_member_spec(system_executor_type_info, { .name = "m_system_executor"});
		std::meta::define_aggregate(^^_system_executor_sequential_type, {system_executor_data_member_spec_info});
	}
}