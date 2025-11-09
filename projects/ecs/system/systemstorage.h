#pragma once

#include "ecs/core/utility.h"

#include <meta>
#include <type_traits>


namespace pulse::ecs
{
	/*
		System Pointer holds the actual pointer to the system instance it self.
		It is instantiated during the call for define_system_storage.
		Each system instantiates this type with it's own pointer type and instance.
		Instance is extracted from the reflection of the system we are passing.
		So basically system pointer instantiation looks like this for each system:
		SystemPointer<decltype(system), [:^^system:]>

		This trick allows us to pass define a pointer member using the decltype(system) and
		assign the defualt value to [:^^system:], which extracted as &system.
		By doing this, we don't need to perform an additional initializion phase to assign
		correct system instances to system pointers. The default values will point to correct
		system address.
	*/
	template<typename _system_type, _system_type _system_instance>
	struct SystemPointer
	{
		using SystemFunctionPointerType = std::add_pointer_t<_system_type>;
		SystemFunctionPointerType m_pointer = _system_instance;
	};

	/*
		Collects all systems under _entity_namespace_info and defines the _system_storage_type
		using the collected systems.
		For each system, system storage will have a SystemPointer<decltype(system), [:system:]>
		member.
	*/
	template<std::meta::info _entity_namespace_info, typename _system_storage_type>
	consteval void define_system_storage()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();
		std::vector<std::meta::info> system_storage_member_infos;

		auto entity_namespace_member_infos = std::meta::members_of(_entity_namespace_info, ctx);
		for(auto member_info : entity_namespace_member_infos)
		{
			if(pulse::ecs::utils::is_system(member_info))
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







