#pragma once
#include "ecs/component/componentstorage.h"
#include "ecs/component/componentindexer.h"
#include "ecs/system/systemstorage.h"
#include "ecs/system/systemexecutorsequential.h"
#include "ecs/entity/entityinfo.h"
#include "ecs/entity/entitystorage.h"
#include "ecs/entity/entity.h"

#include <cstddef>
#include <meta>
#include <type_traits>

namespace pulse::ecs
{

	template<std::meta::info _entity_namespace_info, std::size_t _entity_capacity>
	struct Scene2
	{
		using Entity = std::size_t;
		template<typename _component_type> using ComponentArray = std::array<_component_type, _entity_capacity>;

		struct ComponentArrayStore;
		struct ComponentUtilityStore;
		struct SystemFunctionStore;
		struct SystemFunctionUtilityStore;

		consteval static std::size_t invalid_index() { return static_cast<size_t>(-1); }
		consteval static std::meta::info get_entity_namespace_info() { return _entity_namespace_info; }
		consteval static std::size_t get_entity_capacity() { return _entity_capacity; }
		
		consteval static bool is_component(std::meta::info in_info)
		{
			return std::meta::is_type(in_info);
		}
		
		consteval static bool is_system(std::meta::info in_info)
		{
			return std::meta::is_function(in_info);
		}

		template<
			typename _component_type,
			std::size_t _component_index,
			std::ptrdiff_t _component_array_offset>
		struct ComponentMeta
		{
			consteval static std::meta::info get_component_type_info() { return ^^_component_type; }
			consteval static std::size_t get_component_index() { return _component_index; }
			consteval static std::ptrdiff_t get_component_array_offset() { return _component_array_offset; }
		};

		template<typename ..._component_meta_types>
		struct ComponentUtilityImplementation
		{
			consteval static std::meta::info get_component_meta_info(std::meta::info in_component_type_info)
			{
				std::meta::info component_meta_info = std::meta::info();

				(
			    	((in_component_type_info == _component_meta_types::get_component_type_info())
			        	? (component_meta_info = ^^_component_meta_types, true)
			        	: false
			        )
			        || ...
			    );

				return component_meta_info;
			}

			consteval static std::size_t get_component_index(std::meta::info in_component_type_info)
			{
				std::size_t component_index = invalid_index();

			    (
			    	((in_component_type_info == _component_meta_types::get_component_type_info())
			        	? (component_index = _component_meta_types::get_component_index(), true)
			        	: false
			        )
			        || ...
			    );

				return component_index;
			}

			consteval static std::ptrdiff_t get_component_array_offset(std::meta::info in_component_type_info)
			{
				std::ptrdiff_t component_array_offset = 0;

			    (
			    	((in_component_type_info == _component_meta_types::get_component_type_info())
			        	? (component_array_offset = _component_meta_types::get_component_array_offset(), true)
			        	: false
			        )
			        || ...
			    );

				return component_array_offset;
			}
		};

		template<typename _component_array_store_type>
		static consteval void define_component_array_store()
		{
			std::vector<std::meta::info> component_array_store_member_infos;

			const auto context = std::meta::access_context::unchecked();

			const auto entity_namespace_info = get_entity_namespace_info();
			const auto component_array_template_info = ^^ComponentArray;

			const auto members = std::meta::members_of(entity_namespace_info, context);
			for(auto member : members)
			{
				if(is_component(member))
				{
					const auto component_type_info = member;
					const auto component_array_info = std::meta::substitute(
						component_array_template_info,
						{ component_type_info }
					);

					const auto component_identifier = std::meta::identifier_of(component_type_info);
					const auto component_array_store_member_info = std::meta::data_member_spec(
						component_array_info,
						{ .name = component_identifier }
					);

					component_array_store_member_infos.push_back(component_array_store_member_info);
				}
			}

			const auto component_array_store_info = ^^_component_array_store_type;
			std::meta::define_aggregate(
				component_array_store_info,
				component_array_store_member_infos
			);
		}

		template<typename _component_utility_store_type, typename _component_array_store_type>
		static consteval void define_component_utility_store()
		{
			std::size_t component_index_counter = 0;
			std::vector<std::meta::info> component_utility_implementation_template_argument_infos;

			const auto context = std::meta::access_context::unchecked();
			const auto component_array_store_type_info = ^^_component_array_store_type;
			const auto component_meta_template_type_info = ^^ComponentMeta;

			const auto members = std::meta::members_of(component_array_store_type_info, context);
			for(const auto member : members)
			{
				if(std::meta::is_special_member_function(member))
				{
					continue;
				}

				const auto component_array_member_info = member;
				const auto component_array_type_info = std::meta::type_of(component_array_member_info);
				const auto component_type_info = std::meta::template_arguments_of(component_array_type_info)[0];
				
				const auto component_index_info = std::meta::reflect_constant(component_index_counter++);
				
				const auto component_array_offset = std::meta::offset_of(component_array_member_info);
				const auto component_array_offset_info = std::meta::reflect_constant(component_array_offset.bytes);
				
				const auto component_meta_type_info = std::meta::substitute(
					component_meta_template_type_info,
					{ component_type_info, component_index_info, component_array_offset_info }
				);

				component_utility_implementation_template_argument_infos.push_back(component_meta_type_info);
			}

			const auto component_utility_implementation_template_type_info = ^^ComponentUtilityImplementation;
			const auto component_utility_implementation_type_info = std::meta::substitute(
				component_utility_implementation_template_type_info,
				component_utility_implementation_template_argument_infos
			);
			const auto component_utility_implementation_member_info = std::meta::data_member_spec(
				component_utility_implementation_type_info,
				{ .name = "m_component_utility_implementation" }
			);
			
			const auto component_utility_store_type_info = ^^_component_utility_store_type;
			std::meta::define_aggregate(
				component_utility_store_type_info,
				{ component_utility_implementation_member_info }
			);
		}

		template<
			typename _function_type,
			_function_type _function>
		struct SystemFunction
		{
			consteval static std::meta::info get_function_type_info() { return ^^_function_type; }
			consteval static std::meta::info get_function_pointer_type_info() { return std::meta::add_pointer(get_function_type_info()); }

			using FunctionPointerType = [:get_function_pointer_type_info():];
			FunctionPointerType m_function_pointer = _function;
		};

		template<typename _component_type>
		static const _component_type& get_component(
			const Entity in_entity, 
			const ComponentArrayStore& in_component_array_store)
		{
			constexpr auto component_type_info = ^^_component_type;
			constexpr auto component_array_offset = ComponentUtility::get_component_array_offset(component_type_info);
		
			const auto* component_array_store_ptr = reinterpret_cast<const char*>(&in_component_array_store);
			const auto* component_array_ptr = reinterpret_cast<const ComponentArray<_component_type>*>(component_array_store_ptr + component_array_offset);
			const auto& component = (*component_array_ptr)[in_entity];		
			return component;
		}

		template<
			typename _system_function_type_info,
			typename _function_type_info,
			std::ptrdiff_t _system_function_offset,
			typename ..._function_parameter_types>
		struct SystemFunctionCaller
		{
			static void call(
				const Entity in_entity,
				const SystemFunctionStore& in_system_function_store,
				const ComponentArrayStore& in_component_array_store)
			{
				const auto* system_function_store_ptr = reinterpret_cast<const char*>(&in_system_function_store);
				const auto* system_function_ptr = reinterpret_cast<const _system_function_type_info*>(system_function_store_ptr + _system_function_offset);
			
				std::invoke(
		            system_function_ptr->m_function_pointer,
		            get_component<_function_parameter_types>(in_entity, in_component_array_store)...
		        );
			}
		};

		template<
			typename _system_function_type,
			std::size_t _system_function_index,
			std::ptrdiff_t _system_function_offset>
		struct SystemFunctionMeta
		{
			consteval static std::meta::info get_system_function_type_info() { return ^^_system_function_type; }
			consteval static std::size_t get_system_function_index() { return _system_function_index; }
			consteval static std::ptrdiff_t get_system_function_offset() { return _system_function_offset; }
			
			consteval static std::meta::info get_function_type_info() { return _system_function_type::get_function_type_info(); }
			consteval static std::meta::info get_function_pointer_type_info() { return _system_function_type::get_function_pointer_type_info(); }
		
			consteval static std::meta::info get_system_function_caller_type_info()
			{
				std::vector<std::meta::info> system_function_caller_template_argument_info;
				system_function_caller_template_argument_info.push_back(get_system_function_type_info());
				system_function_caller_template_argument_info.push_back(get_function_type_info());
				system_function_caller_template_argument_info.push_back(std::meta::reflect_constant(get_system_function_offset()));

				const auto function_type_info = get_function_type_info();
				const auto function_parameters_info = std::meta::parameters_of(function_type_info);
				for(const auto function_parameter_info : function_parameters_info)
				{
					const auto function_parameter_type_info = function_parameter_info;
					const auto function_parameter_type_pure_info = std::meta::remove_cvref(function_parameter_type_info);
					system_function_caller_template_argument_info.push_back(function_parameter_type_pure_info);
				}

				const auto system_function_caller_template_type_info = ^^SystemFunctionCaller;
				const auto system_function_caller_type_info = std::meta::substitute(
					system_function_caller_template_type_info,
					system_function_caller_template_argument_info
				);

				return system_function_caller_type_info;
			}
		};

		template<typename _system_function_meta_type>
		static void call_system_function(
			const Entity in_entity,
			const SystemFunctionStore& in_system_function_store,
			const ComponentArrayStore& in_component_array_store)
		{

			constexpr auto system_function_caller_type_info = _system_function_meta_type::get_system_function_caller_type_info();
			using Caller = [:system_function_caller_type_info:];
			Caller::call(in_entity, in_system_function_store, in_component_array_store);
		}

		template<typename ..._system_function_meta_types>
		struct SystemFunctionUtilityImplementation
		{
			static void call_system_functions_sequential(
				const Entity in_entity,
				const SystemFunctionStore& in_system_function_store,
				const ComponentArrayStore& in_component_array_store)
			{
				(call_system_function<_system_function_meta_types>(in_entity, in_system_function_store, in_component_array_store), ...);
			}
		};

		template<typename _system_function_store_type>
		static consteval void define_system_function_store()
		{
			std::vector<std::meta::info> system_function_store_member_infos;

			const auto context = std::meta::access_context::unchecked();

			const auto entity_namespace_info = get_entity_namespace_info();
			const auto system_function_template_info = ^^SystemFunction;

			const auto members = std::meta::members_of(entity_namespace_info, context);
			for(auto member : members)
			{
				if(is_function(member))
				{
					const auto function_info = member;
					const auto function_type_info = std::meta::type_of(function_info);
					const auto system_function_type_info = std::meta::substitute(
						system_function_template_info, 
						{ function_type_info, function_info }
					);

					const auto function_identifier = std::meta::identifier_of(function_info);
					const auto system_function_store_member_info = std::meta::data_member_spec(
						system_function_type_info,
						{ .name = function_identifier }
					);

					system_function_store_member_infos.push_back(system_function_store_member_info);
				}
			}

			const auto system_function_store_info = ^^_system_function_store_type;
			std::meta::define_aggregate(
				system_function_store_info,
				system_function_store_member_infos
			);
		}

		template<typename _system_function_utility_store_type, typename _system_function_store_type>
		static consteval void define_system_function_utility_store()
		{
			std::size_t system_function_index_counter = 0;
			std::vector<std::meta::info> system_function_utility_implementation_template_argument_infos;

			const auto context = std::meta::access_context::unchecked();
			const auto system_function_store_type_info = ^^_system_function_store_type;
			const auto system_function_meta_template_type_info = ^^SystemFunctionMeta;

			const auto members = std::meta::members_of(system_function_store_type_info, context);
			for(const auto member : members)
			{
				if(std::meta::is_special_member_function(member))
				{
					continue;
				}

				const auto system_function_member_info = member;
				const auto system_function_type_info = std::meta::type_of(system_function_member_info);

				const auto system_function_index_info = std::meta::reflect_constant(system_function_index_counter++);
				
				const auto system_function_offset = std::meta::offset_of(system_function_member_info);
				const auto system_function_offset_info = std::meta::reflect_constant(system_function_offset.bytes);
				
				const auto component_meta_type_info = std::meta::substitute(
					system_function_meta_template_type_info,
					{ system_function_type_info, system_function_index_info, system_function_offset_info }
				);

				system_function_utility_implementation_template_argument_infos.push_back(component_meta_type_info);
			}

			const auto system_function_utility_implementation_template_type_info = ^^SystemFunctionUtilityImplementation;
			const auto system_function_utility_implementation_type_info = std::meta::substitute(
				system_function_utility_implementation_template_type_info,
				system_function_utility_implementation_template_argument_infos
			);
			const auto system_function_utility_implementation_member_info = std::meta::data_member_spec(
				system_function_utility_implementation_type_info,
				{ .name = "m_system_function_utility_implementation" }
			);
			
			const auto system_function_utility_store_type_info = ^^_system_function_utility_store_type;
			std::meta::define_aggregate(
				system_function_utility_store_type_info,
				{ system_function_utility_implementation_member_info }
			);
		}

		consteval
		{
			define_component_array_store<ComponentArrayStore>();
			define_component_utility_store<ComponentUtilityStore, ComponentArrayStore>();
			
			define_system_function_store<SystemFunctionStore>();
			define_system_function_utility_store<SystemFunctionUtilityStore, SystemFunctionStore>();
		}

		ComponentArrayStore m_component_array_store;
		ComponentUtilityStore m_component_utility_store;
		SystemFunctionStore m_system_function_store;
		SystemFunctionUtilityStore m_system_function_utility_store;

		using ComponentUtility = decltype(m_component_utility_store.m_component_utility_implementation);
		using SystemFunctionUtility = decltype(m_system_function_utility_store.m_system_function_utility_implementation);
	
		void execute_systems(const Entity in_entity)
		{
			SystemFunctionUtility::call_system_functions_sequential(in_entity, m_system_function_store, m_component_array_store);
		}

		template<typename _component_type>
		const _component_type& get_component(const Entity in_entity)
		{
			return get_component<_component_type>(in_entity, m_component_array_store);
		}
	};



	template<std::meta::info _entity_namespace_info, size_t _entity_capacity>
	struct Scene
	{
		struct EntityInfoType;
		struct EntityStorage;
	    struct ComponentStorage;
	    struct ComponentIndexer;
	    struct SystemStorage;
	    struct SystemExecutor;

		static constexpr inline size_t s_entity_capacity = _entity_capacity;

		consteval
		{
			constexpr auto entity_capacity = s_entity_capacity;

		    pulse::ecs::define_component_storage<_entity_namespace_info, ComponentStorage, entity_capacity>();
		    pulse::ecs::define_system_storage<_entity_namespace_info, SystemStorage>();
		    pulse::ecs::define_system_executor_sequential<^^SystemStorage, ^^ComponentStorage, SystemExecutor>();

		    pulse::ecs::define_component_indexer<^^ComponentStorage, ComponentIndexer>();
			pulse::ecs::define_entity_info<_entity_namespace_info, EntityInfoType>();
		    pulse::ecs::define_entity_storage<EntityStorage, EntityInfoType, entity_capacity>();
		}

	public:
		template<typename _component_type>
		constexpr size_t get_component_index()
		{
			return decltype(m_component_indexer.m_component_indexer)::template get_component_index<_component_type>();
		}

		template<typename _component_type>
		void add_component(const size_t in_entity_index)
		{
			const auto component_index = get_component_index<_component_type>();
			m_entity_storage.m_entities[in_entity_index].m_component_bitset.set(component_index);
		}

		template<typename _component_type>
		void remove_component(const size_t in_entity_index)
		{
			const auto component_index = get_component_index<_component_type>();
			m_entity_storage.m_entities[in_entity_index].m_component_bitset.reset(component_index);
		}

		template<typename _component_type>
		bool has_component(const size_t in_entity_index)
		{
			const auto component_index = get_component_index<_component_type>();
			return m_entity_storage.m_entities[in_entity_index].m_component_bitset[component_index];
		}

		void execute_systems_for_entity(pulse::ecs::Entity in_entity)
		{
			m_system_executor.m_system_executor.execute_systems(in_entity, m_system_storage, m_component_storage);
		}

		void debug_systems_for_entity(pulse::ecs::Entity in_entity)
		{
			m_system_executor.m_system_executor.debug_systems(in_entity, m_system_storage, m_component_storage);
		}

	public:
		EntityStorage m_entity_storage;
		ComponentStorage m_component_storage;
		ComponentIndexer m_component_indexer;
		SystemStorage m_system_storage;
		SystemExecutor m_system_executor;
	};
}