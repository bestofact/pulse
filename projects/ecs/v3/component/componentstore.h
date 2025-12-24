#pragma once

#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/utils/detail.h"
#include <bitset>
#include <cstddef>
#include <meta>
#include <array>

#define UNTIL_COMPONENT_TYPE_INFO_MATCH(ComponentTypeInfo, ComponentTypes, TrueBody, FalseBody)\
    ((std::meta::is_same_type(ComponentTypeInfo, ^^ComponentTypes)\
		 ? (TrueBody), true\
		 : (FalseBody), false)\
		|| ...\
		)

#define FOR_EACH_VARIADIC_ARGUMENT(Body)\
    ((Body), ...)

namespace pulse::ecs
{
	/**
	 *
	**/
	template<
	    template <pulse::ecs::concepts::Component T> typename COMPONENT_POOL_TEMPLATE_TYPE,
		pulse::ecs::concepts::Component ...COMPONENT_TYPES>
	struct ComponentStore final
	{
#pragma region meta

        consteval static std::meta::info get_component_pool_template_type_info()
        {
            return ^^COMPONENT_POOL_TEMPLATE_TYPE;
        }

        consteval static std::size_t get_component_count()
        {
           	return sizeof...(COMPONENT_TYPES);
        }

        consteval static std::meta::info get_first_component_type_info()
        {
            const std::array<std::meta::info, get_component_count()> componentTypeInfos = {
                ^^COMPONENT_TYPES...
            };
            return componentTypeInfos[0];
        }

        consteval static std::meta::info get_entity_type_info()
        {
            const auto firstComponentTypeInfo = get_first_component_type_info();
            return pulse::ecs::utils::get_entity_type_info(firstComponentTypeInfo);
        }

        consteval static std::size_t get_entity_capacity_value()
        {
            const auto firstComponentTypeInfo = get_first_component_type_info();
            const auto capacityMemberInfo = pulse::ecs::utils::get_entity_capacity_member_info(firstComponentTypeInfo);
            const auto value = std::meta::extract<std::size_t>(capacityMemberInfo);
            return value;
        }

        consteval static std::size_t get_component_index(std::meta::info in_componentTypeInfo)
        {
            std::size_t index = 0, dummy = 0;
            UNTIL_COMPONENT_TYPE_INFO_MATCH(std::meta::decay(in_componentTypeInfo), COMPONENT_TYPES, ++dummy, ++index);
            return index;
        }

        struct ComponentStoreImpl;
        consteval static std::meta::info get_component_store_impl_type_info()
        {
            return ^^ComponentStoreImpl;
        }

        consteval static std::meta::info get_component_pool_type_info(std::meta::info in_componentTypeInfo)
        {
            return std::meta::substitute(get_component_pool_template_type_info(), { std::meta::decay(in_componentTypeInfo) });
        }

        consteval static std::meta::data_member_options get_component_pool_options(std::meta::info in_componentTypeInfo)
        {
            return std::meta::data_member_options{
                .name = { std::meta::identifier_of(std::meta::decay(in_componentTypeInfo)) }
            };
        }

        consteval static std::array<std::meta::info, get_component_count()> get_component_pool_data_member_specs()
        {
			constexpr std::array<std::meta::info, get_component_count()> componentPoolDataMemberSpecs =
			{
				std::meta::data_member_spec(
				    get_component_pool_type_info(^^COMPONENT_TYPES),
					get_component_pool_options(^^COMPONENT_TYPES)
				)...
			};

			return componentPoolDataMemberSpecs;
        }

		consteval
		{
            std::meta::define_aggregate(get_component_store_impl_type_info(), get_component_pool_data_member_specs());
		}

        consteval static std::meta::info get_component_pool_member_info(std::meta::info in_componentTypeInfo)
        {
            constexpr auto ctx = std::meta::access_context::unchecked();
            const auto componentPoolTypeInfo = get_component_pool_type_info(in_componentTypeInfo);

            const auto members = std::meta::nonstatic_data_members_of(get_component_store_impl_type_info(), ctx);
            for(const auto member : members)
            {
                const auto memberType = std::meta::type_of(member);
                if(memberType == componentPoolTypeInfo)
                {
                    return member;
                }
            }

            return std::meta::info();
        }

        consteval static std::ptrdiff_t get_component_pool_member_offset(std::meta::info in_componentTypeInfo)
        {
            const auto memberInfo = get_component_pool_member_info(in_componentTypeInfo);
            const auto memberInfoOffset = std::meta::offset_of(memberInfo);
            return memberInfoOffset.bytes;
        }

	//private:
		[:get_component_store_impl_type_info():] m_componentStoreImpl;

#pragma endregion meta

    public:

        template<pulse::ecs::concepts::Component COMPONENT_TYPE>
        const auto& get_component_pool() const
        {
            constexpr auto memberInfo = get_component_pool_member_info(^^COMPONENT_TYPE);
            return m_componentStoreImpl.[:memberInfo:];
        }

        template<pulse::ecs::concepts::Component COMPONENT_TYPE>
        auto& get_component_pool()
        {
            constexpr auto memberInfo = get_component_pool_member_info(^^COMPONENT_TYPE);
            return m_componentStoreImpl.[:memberInfo:];
        }

        template<pulse::ecs::concepts::Component COMPONENT_TYPE>
        const auto& get_component(
            const [:pulse::ecs::utils::get_entity_type_info(^^COMPONENT_TYPE):] in_entity)
        {
            const auto& componentPool = get_component_pool<COMPONENT_TYPE>();
            const auto& componentArray = componentPool.get_component_array();
            return componentArray[in_entity.get_index()];
        }

        template<pulse::ecs::concepts::Component COMPONENT_TYPE>
        auto& get_component_mutable(
            const [:get_entity_type_info():] in_entity)
        {
            auto& componentPool = get_component_pool<COMPONENT_TYPE>();
            auto& componentArray = componentPool.get_component_array();
            return componentArray[in_entity.get_index()];
        }

        template<pulse::ecs::concepts::Component... TARGET_COMPONENT_TYPES>
        std::bitset<get_entity_capacity_value()> get_component_bitset() const
        {
            std::bitset<get_entity_capacity_value()> componentBitset;
            componentBitset.set();

            FOR_EACH_VARIADIC_ARGUMENT(
                componentBitset &= get_component_pool<TARGET_COMPONENT_TYPES>().get_component_bitset()
            );

            return componentBitset;
        }

        template<pulse::ecs::concepts::Component... TARGET_COMPONENT_TYPES>
        void set_component_bitset(
            const [:get_entity_type_info():] in_entity)
        {
            FOR_EACH_VARIADIC_ARGUMENT(
                get_component_pool<TARGET_COMPONENT_TYPES>().get_component_bitset().set(in_entity.get_index())
            );
        }

        template<pulse::ecs::concepts::Component... TARGET_COMPONENT_TYPES>
        void reset_component_bitset(
            const [:get_entity_type_info():] in_entity)
        {
            FOR_EACH_VARIADIC_ARGUMENT(
                get_component_pool<TARGET_COMPONENT_TYPES>().get_component_bitset().reset(in_entity.get_index())
            );
        }

        template<pulse::ecs::concepts::Component... TARGET_COMPONENT_TYPES>
        void set_component_bitset()
        {
            FOR_EACH_VARIADIC_ARGUMENT(
                get_component_pool<TARGET_COMPONENT_TYPES>().get_component_bitset().set()
            );
        }

        template<pulse::ecs::concepts::Component... TARGET_COMPONENT_TYPES>
        void reset_component_bitset()
        {
            FOR_EACH_VARIADIC_ARGUMENT(
                get_component_pool<TARGET_COMPONENT_TYPES>().get_component_bitset().reset()
            );
        }
	};
}

#undef FOR_EACH_VARIADIC_ARGUMENT
#undef UNTIL_COMPONENT_TYPE_INFO_MATCH
