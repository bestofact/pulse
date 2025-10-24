#pragma once
#include <iostream>
#include <meta>

namespace pulse::ecs
{
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
}