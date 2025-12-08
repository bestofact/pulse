#pragma once


#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/concepts/componentpoolconcept.h"
#include <cstddef>
#include <meta>
#include <array>

#define UNTIL_COMPONENT_TYPE_INFO_MATCH(ComponentTypeInfo, ComponentTypes, TrueBody, FalseBody)\
    ((std::meta::is_same_type(ComponentTypeInfo, ^^ComponentTypes)\
		 ? (TrueBody), true\
		 : (FalseBody), false)\
		|| ...\
		)

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

	private:
		[:get_component_store_impl_type_info():] m_componentStoreImpl;
	public:

    	consteval std::meta::info get_component_pool_member_info(std::meta::info in_componentTypeInfo) const
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

        template<std::meta::info COMPONENT_TYPE>
        auto get_component_pool_ref()
            -> typename [:std::meta::add_lvalue_reference(get_component_pool_type_info(COMPONENT_TYPE)):]
        {
            constexpr auto componentPoolPtrTypeInfo = std::meta::add_pointer(get_component_pool_type_info(COMPONENT_TYPE));
            const auto memberInfo = get_component_pool_member_info(COMPONENT_TYPE);
            const auto memberOffsetInfo = std::meta::offset_of(memberInfo);
            auto* memberPtr = reinterpret_cast<char*>(&m_componentStoreImpl) + memberOffsetInfo.bytes;
            auto* memberPtrTyped = reinterpret_cast<typename [:componentPoolPtrTypeInfo:]>(memberPtr);
            return *memberPtrTyped;
        }

        template<std::meta::info COMPONENT_TYPE>
        auto get_component_pool_ref() const
            -> typename [:std::meta::add_const(std::meta::add_lvalue_reference(get_component_pool_type_info(COMPONENT_TYPE))):]
        {
            constexpr auto componentPoolPtrTypeInfo = std::meta::add_const(std::meta::add_pointer(get_component_pool_type_info(COMPONENT_TYPE)));
            const auto memberInfo = get_component_pool_member_info(COMPONENT_TYPE);
            const auto memberOffsetInfo = std::meta::offset_of(memberInfo);
            auto* memberPtr = reinterpret_cast<const char*>(&m_componentStoreImpl) + memberOffsetInfo.bytes;
            auto* memberPtrTyped = reinterpret_cast<const typename [:componentPoolPtrTypeInfo:]>(memberPtr);
            return *memberPtrTyped;
        }

#pragma endregion meta

        template<pulse::ecs::concepts::Component COMPONENT_TYPE>
        constexpr const [:get_component_pool_type_info(^^COMPONENT_TYPE):]& get_component_pool() const
        {
            return get_component_pool_ref<^^COMPONENT_TYPE>();
        }

        template<pulse::ecs::concepts::Component COMPONENT_TYPE>
        constexpr [:get_component_pool_type_info(^^COMPONENT_TYPE):]& get_component_pool()
        {
            return get_component_pool_ref<^^COMPONENT_TYPE>();
        }
	};
}

#undef UNTIL_COMPONENT_TYPE_INFO_MATCH
