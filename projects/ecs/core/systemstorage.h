#pragma once
#include <meta>
#include <type_traits>

namespace pulse::ecs
{
	template<typename _system_type, _system_type _system_instance>
	struct SystemPointer
	{
		using SystemFunctionPointerType = std::add_pointer_t<_system_type>;
		SystemFunctionPointerType m_pointer = _system_instance;
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
}







