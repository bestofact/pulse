#pragma once
#include <cstddef>
#include <meta>
#include <type_traits>
#include <vector>

#define PULSE_ECS_DEFINE_STORAGE(_entity_namespace, _entity_count)\
namespace pulse::ecs\
{\
    namespace _entity_namespace\
    {\
        namespace storage\
        {\
            struct Type;\
            static inline constexpr size_t s_entity_count = _entity_count;\
        }\
    }\
}\
consteval\
{\
    constexpr auto ctx = std::meta::access_context::current();\
    constexpr auto storage_type_info = ^^pulse::ecs::_entity_namespace::storage::Type;\
    constexpr size_t buffer = 1000;\
    auto members = pulse::ecs::generate_storage_members<ctx, ^^pulse::ecs::_entity_namespace, _entity_count, buffer>();\
    std::meta::define_aggregate(storage_type_info, members);\
}\
//

namespace pulse::ecs
{
	template<typename _system_type>
	struct SystemContainer final
	{
		void Call() const
		{
			m_system_ptr();
		}

		std::add_pointer_t<_system_type> m_system_ptr = nullptr;
	};

	struct SystemContainerPointer final
	{
		ptrdiff_t m_offset = 0;
		std::meta::info m_info = {};
	};

    // Collect all namespace members under a reflection.
   	template<std::meta::access_context _ctx, std::meta::info _parent, size_t _buffer>
	struct NamespaceCollector
	{
		constexpr NamespaceCollector() : m_data()
		{
			size_t data_index = 0;
			auto members = std::meta::members_of(_parent, _ctx);
			for(auto member : members)
			{
				if(std::meta::is_namespace(member))
				{
					m_data[data_index++] = member;
				}
			}
		}

		static constexpr size_t m_buffer = _buffer;
		std::array<std::meta::info, m_buffer> m_data;
	};

	// Collect all component members under a reflection
	template<std::meta::access_context _ctx, std::meta::info _parent, size_t _buffer>
	struct ComponentCollector
	{
		constexpr ComponentCollector() : m_data()
		{
			size_t data_index = 0;
			auto members = std::meta::members_of(_parent, _ctx);
			for(auto member : members)
			{
				if(std::meta::is_type(member))
				{
					m_data[data_index++] = member;
				}
			}
		}

		static constexpr size_t m_buffer = _buffer;
		std::array<std::meta::info, m_buffer> m_data;
	};

	// Collect all system members under a reflection
	template<std::meta::access_context _ctx, std::meta::info _parent, size_t _buffer>
	struct SystemCollector
	{
		constexpr SystemCollector() : m_data()
		{
			size_t data_index = 0;
			auto members = std::meta::members_of(_parent, _ctx);
			for(auto member : members)
			{
				if(std::meta::is_function(member))
				{
					m_data[data_index++] = member;
				}
			}
		}

		static constexpr size_t m_buffer = _buffer;
		std::array<std::meta::info, m_buffer> m_data;
	};

	// Collect storage members (components&systems) under a reflection.
	template<std::meta::access_context _ctx, std::meta::info _entity_namespace, size_t _entity_count, size_t _buffer>
	struct StorageMemberGenerator
	{
		constexpr StorageMemberGenerator() : m_data()
		{
			size_t storage_index = 0;
			constexpr auto components = ComponentCollector<_ctx, _entity_namespace, _buffer>();
			constexpr auto systems = SystemCollector<_ctx, _entity_namespace, _buffer>();

			// Add components to storage members
			for(auto component : components.m_data)
			{
				if(component != std::meta::info{})
				{
					m_data[storage_index++] = std::meta::data_member_spec(
						std::meta::substitute(^^std::array, {component, std::meta::reflect_constant(_entity_count)}),
						{
							.name = std::meta::identifier_of(component)
						}
					);
				}
			}

			// Skip a member index for system offset data members, accessing it before systems will be cache friendly.
			const size_t system_offset_data_member_index = storage_index++;

			// Add systems.
			size_t system_count = 0;
			for(auto system : systems.m_data)
			{
				if(system != std::meta::info{})
				{
					m_data[storage_index++] = std::meta::data_member_spec(
						std::meta::substitute(^^pulse::ecs::SystemContainer, { std::meta::type_of(system) }),
						{
							.name = std::meta::identifier_of(system)
						}
					);
					++system_count;
				}
			}

			// Add system member offsets data member.
			m_data[system_offset_data_member_index] = std::meta::data_member_spec(
				std::meta::substitute(^^std::array, {^^SystemContainerPointer, std::meta::reflect_constant(system_count)}),
				{
					.name = "m_system_member_offsets"
				}
			);
		}

		static constexpr size_t m_buffer = _buffer;
		std::array<std::meta::info, m_buffer> m_data;
	};

	// Collects member info in a storage.
	template<std::meta::access_context _ctx, std::meta::info _storage_type, size_t _buffer>
	struct StorageMemberCollector
	{
	    constexpr StorageMemberCollector() : m_data()
	    {
	        size_t index = 0;
	        auto members = std::meta::members_of(_storage_type, _ctx);
	        for(auto member : members)
	        {
	            m_data[index++] = member;
	        }
	    }

	    std::array<std::meta::info, _buffer> m_data;
	};

	template<std::meta::info _member>
	struct OffsetOf
	{
	    constexpr OffsetOf() : m_offset(std::meta::offset_of(_member))
	    {
	    }

	    std::meta::member_offset m_offset;
	};

	template<std::meta::info _system, std::meta::info _member>
	struct SystemContainerPointerGenerator
	{
		constexpr SystemContainerPointerGenerator() : m_data()
		{
			constexpr auto member_offset = OffsetOf<_member>();
			m_data.m_offset = member_offset.m_offset.bytes;
			m_data.m_info = _system;
		}

		SystemContainerPointer m_data;
	};

/*
	// Get system container pointer from stroge and member offset to it.
	template<typename _storage_type, typename _system_type>
	SystemContainer<_system_type>* get_storage_system_container(_storage_type& out_storage, const ptrdiff_t in_offset)
	{
		return reinterpret_cast<SystemContainer<_system_type>*>(reinterpret_cast<char*>((&out_storage)) + in_offset);
	}

	// Get system container pointer from stroge and member offset to it.
	template<typename _storage_type, std::meta::info _system_info>
	SystemContainer<typename [:std::meta::type_of(_system_info):]>* get_storage_system_container(_storage_type& out_storage, const ptrdiff_t in_offset)
	{
		return reinterpret_cast<SystemContainer<typename [:std::meta::type_of(_system_info):]>*>(reinterpret_cast<char*>((&out_storage)) + in_offset);
	}

	*/

    // This returns a vector of members' reflection info for a storage.
    template<
        std::meta::access_context _ctx,
        std::meta::info _entity_namespace,
        size_t _entity_count,
        size_t _buffer>
    consteval std::vector<std::meta::info> generate_storage_members()
    {
        constexpr auto storageMembers = pulse::ecs::StorageMemberGenerator<_ctx, _entity_namespace, _entity_count,  _buffer>();
        std::vector<std::meta::info> members;
        template for(constexpr auto member : storageMembers.m_data)
        {
            if(member != std::meta::info{})
            {
                members.push_back(member);
            }
        }
        return members;
    }

/*
    template<std::meta::access_context _ctx, std::meta::info _entity_namespace, typename _storage_type, size_t _buffer>
    void construct_storage(_storage_type& out_storage)
    {
    	constexpr auto storage = pulse::ecs::StorageMemberCollector<_ctx, ^^_storage_type, _buffer>();
    	constexpr auto systems = pulse::ecs::SystemCollector<_ctx, _entity_namespace, _buffer>();

    	size_t system_index = 0;
	    template for(constexpr auto member : storage.m_data)
	    {
	        if constexpr(member != std::meta::info{} && !std::meta::is_special_member_function(member))
	        {
	            template for(constexpr auto system : systems.m_data)
	            {
	                if constexpr(system != std::meta::info{})
	                {
	                    if constexpr(std::meta::identifier_of(system) == std::meta::identifier_of(member))
	                    {
	                    	//auto system_type_info = std::meta::type_of(member);
	                        constexpr auto member_offset = pulse::ecs::OffsetOf<member>();	                        
	                        
	                        auto* system_container = get_storage_system_container<_storage_type, >(out_storage, member_offset.m_offset.bytes);
	                  		system_container->m_system_function_ptr = &[:system:];
	                 
	                 		constexpr auto system_pointer = SystemPointerGenerator<system, member>();
	                    	out_storage.m_system_member_offsets[system_index++] = system_pointer.m_data;
	                    }
	                }
	            }
	        }
	    }
    }

    template<typename _storage_type>
    void execute_storage_systems(_storage_type& io_storage)
    {
    	for(auto offset : io_storage.m_system_member_offsets)
	    {
	        auto* system_container = pulse::ecs::get_storage_system_container<decltype(io_storage), offset.m_info>(io_storage, offset.m_offset);
	        system_container->Call();
	    }
    }

    */
}

#define PULSE_ECS_CONSTRUCT_STORAGE(_identifier, _entity_namespace, _buffer)\
pulse::ecs::_entity_namespace::storage::Type _identifier;\
{\
	constexpr auto _access_context = std::meta::access_context::current();\
    pulse::ecs::construct_storage<_access_context, ^^pulse::ecs::_entity_namespace, decltype(_identifier), _buffer>(_identifier);\
}\
//
