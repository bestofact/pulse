#pragma once

#include "ecs/access/reader.h"
#include "ecs/access/writer.h"

#include <iostream>
#include <meta>

namespace pulse::ecs
{

	/*
		Executes a system with given system execution info.
		What it does basically is, taking the address of the system storage and offseting 
		as the system pointer offset that is retrieved from _system_execution_info.

		Each system in a storage instantiates it's own system execution info (as each pointer offset
		is different for each system in a storage). Thus this execute_system is instantiated for
		each system as well. 
	
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
	*/
}