#pragma once

#include <meta>

namespace pulse::ecs
{
	template<typename ... _component_types>
	struct ComponentIndexer
	{
		template<typename _component_type>
		static consteval size_t get_component_index()
		{
			bool found = false;
			size_t index = 0;
			([&index, &found]()
			{
				if (!found && std::is_same_v<_component_type, _component_types>)
				{
					++index;
				}
				else
				{
					found = true;
				}
			}(),
			...);
			return index;
		}
	};

	template<std::meta::info _component_storage_type_info, typename _component_indexer_type>
	consteval void define_component_indexer()
	{
		constexpr auto ctx = std::meta::access_context::unchecked();

		std::vector<std::meta::info> component_indexer_template_argument_infos;

		auto component_storage_member_infos = std::meta::nonstatic_data_members_of(_component_storage_type_info, ctx);
		for(auto member_info : component_storage_member_infos)
		{
			auto component_type_info = std::meta::template_arguments_of(std::meta::type_of(member_info))[0];
			component_indexer_template_argument_infos.push_back(component_type_info);		
		}

		auto component_indexer_type_info = std::meta::substitute(^^ComponentIndexer, component_indexer_template_argument_infos);
		auto component_indexer_data_member_spec_info = std::meta::data_member_spec(component_indexer_type_info, { .name = "m_component_indexer"});
		std::meta::define_aggregate(^^_component_indexer_type, {component_indexer_data_member_spec_info});
	}
}