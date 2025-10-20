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
	struct SystemContainerBase
	{
		virtual ~SystemContainerBase() = default;
		virtual void Call() = 0;

		void(*m_system_function_ptr)() = nullptr;
	};

	template<typename _system_type>
	struct SystemContainer final : public SystemContainerBase
	{
		using SystemType = std::remove_pointer_t<_system_type>;

		virtual void Call() override
		{
			auto fn = reinterpret_cast<SystemType*>(m_system_function_ptr);
			fn();
		}
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
						std::meta::substitute(^^pulse::ecs::SystemContainer, { std::meta::add_pointer(std::meta::type_of(system)) }),
						{
							.name = std::meta::identifier_of(system)
						}
					);
					++system_count;
				}
			}

			// Add system member offsets data member.
			m_data[system_offset_data_member_index] = std::meta::data_member_spec(
				std::meta::substitute(^^std::array, {^^ptrdiff_t, std::meta::reflect_constant(system_count)}),
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

	// Get system container pointer from stroge and member offset to it.
	template<typename _storage_type>
	SystemContainerBase* get_storage_system_container(_storage_type& out_storage, const ptrdiff_t in_offset)
	{
		return reinterpret_cast<SystemContainerBase*>(reinterpret_cast<char*>((&out_storage)) + in_offset);
	}

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

    template<std::meta::access_context _ctx, std::meta::info _entity_namespace, typename _storage_type, size_t _buffer>
    void construct_storage(_storage_type& out_storage)
    {
    	constexpr auto storage = pulse::ecs::StorageMemberCollector<_ctx, ^^_storage_type, _buffer>();
    	constexpr auto systems = pulse::ecs::SystemCollector<_ctx, _entity_namespace, _buffer>();

    	struct SystemInstanceInfo final
    	{
    		ptrdiff_t m_storage_offset = 0;
    		void(*m_system_function_ptr)()  = nullptr;
    	};

    	std::vector<SystemInstanceInfo> system_instance_info;
	    template for(constexpr auto member : storage.m_data)
	    {
	        if constexpr(member != std::meta::info{} && !std::meta::is_special_member_function(member) && !std::meta::is_template(member))
	        {
	            template for(constexpr auto system : systems.m_data)
	            {
	                if constexpr(system != std::meta::info{})
	                {
	                    if constexpr(std::meta::identifier_of(system) == std::meta::identifier_of(member))
	                    {
	                        auto member_offset = pulse::ecs::OffsetOf<member>();
	                        auto system_function_ptr = &[:system:];
	                        system_instance_info.push_back({member_offset.m_offset.bytes, system_function_ptr});
	                    }
	                }
	            }
	        }
	    }

	    for(size_t system_index = 0; system_index < system_instance_info.size(); ++system_index)
	    {
	    	const auto system_instance = system_instance_info[system_index];
	    	out_storage.m_system_member_offsets[system_index] = system_instance.m_storage_offset;

	    	auto* system_container = get_storage_system_container<_storage_type>(out_storage, system_instance.m_storage_offset);
	        system_container->m_system_function_ptr = system_instance.m_system_function_ptr;
	    }
    }

    template<typename _storage_type>
    void execute_storage_systems(_storage_type& io_storage)
    {
    	for(auto offset : io_storage.m_system_member_offsets)
	    {
	        auto* system_container = pulse::ecs::get_storage_system_container<decltype(io_storage)>(io_storage, offset);
	        system_container->Call();
	    }
    }
}

#define PULSE_ECS_CONSTRUCT_STORAGE(_identifier, _entity_namespace, _buffer)\
pulse::ecs::_entity_namespace::storage::Type _identifier;\
{\
	constexpr auto _access_context = std::meta::access_context::current();\
    pulse::ecs::construct_storage<_access_context, ^^pulse::ecs::_entity_namespace, decltype(storage), _buffer>(_identifier);\
}\
//
