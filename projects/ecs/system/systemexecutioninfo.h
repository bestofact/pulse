#pragma once

#include "ecs/access/reader.h"
#include "ecs/access/writer.h"
#include "ecs/entity/entity.h"
#include "ecs/system/systemstorage.h"

#include <cstddef>
#include <meta>
#include <iostream>
#include <type_traits>

namespace pulse::ecs
{
	/*
		Contains the required information to execute a system.
		This struct is instantiated during compile-time for each system in a storage.
		Stores the necessary system type information to execute the system and correct
		the pointer offset for the SystemPointer member in the system storage.
		While executing a system, we are rely on this execution info.
	*/
	template<typename _system_pointer_type, std::ptrdiff_t _system_pointer_member_offset>
	struct SystemExecutionInfo2
	{
		using SystemPointerType = _system_pointer_type;
		using SystemFunctionPointerType = SystemPointerType::SystemFunctionPointerType;
		using SystemType = std::remove_pointer_t<SystemFunctionPointerType>;
		static constexpr inline std::ptrdiff_t s_system_pointer_member_offset = _system_pointer_member_offset;
	};


	enum class ESystemParameterType
	{
		Read = 1 << 0,
		Write = 1 << 1
	};
	using SystemParameterFlag = uint32_t;

	template<typename _system_type>
	constexpr SystemParameterFlag get_system_parameter_flags()
	{
		constexpr auto reader_template_type_info = ^^pulse::ecs::Reader;
		constexpr auto writer_template_type_info = ^^pulse::ecs::Writer;
		constexpr auto system_type_info = ^^_system_type;
		auto system_parameter_infos = std::meta::parameters_of(system_type_info);

		SystemParameterFlag flags = 0;
		for(auto system_parameter_info : system_parameter_infos)
		{
			auto system_parameter_template_info = std::meta::template_of(system_parameter_info);
			if(std::meta::is_same_type(reader_template_type_info, system_parameter_template_info))
			{
				flags |= static_cast<SystemParameterFlag>(ESystemParameterType::Read);
			}
			else if(std::meta::is_same_type(writer_template_type_info, system_parameter_info))
			{
				flags |= static_cast<SystemParameterFlag>(ESystemParameterType::Write);
			}
		}

		return flags;
	}


	template<
		typename _system_storage_type,
		typename _component_storage_type,
		typename _system_pointer_type, 
		std::ptrdiff_t _system_pointer_member_offset>
	struct SystemExecutor
	{
		using SystemPointerType = _system_pointer_type;
		using SystemFunctionPointerType = SystemPointerType::SystemFunctionPointerType;
		using SystemType = std::remove_pointer_t<SystemFunctionPointerType>;

		static const SystemPointerType* get_system_function_pointer(const _system_storage_type& in_system_storage)
		{
			char* system_storage_char_ptr = reinterpret_cast<char*>(&const_cast<_system_storage_type&>(in_system_storage));
			auto* system_pointer = reinterpret_cast<SystemPointerType*>(system_storage_char_ptr + _system_pointer_member_offset);
			return system_pointer;
		}

		static void execute(
			const pulse::ecs::Entity in_entity,
			const _system_storage_type& in_system_storage, 
			const _component_storage_type& in_component_storage)
		{
			constexpr auto system_parameter_flags = get_system_parameter_flags<std::remove_pointer_t<SystemType>>();
			constexpr auto read_flag = static_cast<SystemParameterFlag>(ESystemParameterType::Read);
			constexpr auto write_flag = static_cast<SystemParameterFlag>(ESystemParameterType::Write);
			constexpr auto read_write_flag = (read_flag | write_flag);

			const auto* system_pointer = get_system_function_pointer(in_system_storage);

			//system_pointer->m_pointer();
			
			//std::cout << std::meta::is_function_type(^^SystemType) << std::endl;
			
		}

		static void debug(
			const pulse::ecs::Entity in_entity,
			const _system_storage_type& in_system_storage, 
			const _component_storage_type& in_component_storage)
		{
				
		}
	};
}