#pragma once

#include <meta>
#include "ecs/v3/component/componentpool.h"
#include "ecs/v3/component/componentstore.h"
#include "ecs/v3/entity/entitycapacity.h"
#include "ecs/v3/concepts/componentconcept.h"
#include "ecs/v3/concepts/componentpoolconcept.h"
#include "ecs/v3/utils/detail.h"
#include "ecs/v3/entity/entity.h"
#include "ecs/v3/concepts/entityconcept.h"

namespace pulse::ecs
{
    template<
        std::meta::info NAMESPACE_INFO>
    struct World
    {
#pragma region meta
        consteval static std::meta::info get_namespace_info()
        {
            return NAMESPACE_INFO;
        }

        consteval static std::meta::info get_component_pool_template_info()
        {
            return ^^pulse::ecs::ComponentPool;
        }

        consteval static std::meta::info get_component_store_template_info()
        {
            return ^^pulse::ecs::ComponentStore;
        }

        consteval static std::meta::info get_entity_type_info()
        {
            return pulse::ecs::utils::get_entity_type_info(get_namespace_info());
        }

        consteval static std::vector<std::meta::info> get_component_type_infos()
        {
            return pulse::ecs::utils::get_component_type_infos(get_namespace_info());
        }

        consteval static std::size_t get_component_count()
        {
            return get_component_type_infos().size();
        }

        consteval static std::size_t get_component_store_template_args_count()
        {
            return 1 + get_component_count();
        }

        consteval static auto get_component_store_template_args()
            -> std::array<std::meta::info, get_component_store_template_args_count()>
        {
            std::array<std::meta::info, get_component_store_template_args_count()> result;
            result[0] = get_component_pool_template_info();

            constexpr std::size_t componentCount = get_component_count();
            const auto componentTypeInfos = get_component_type_infos();
            for(std::size_t i = 0; i < componentCount; ++i)
            {
                result[i+1] = componentTypeInfos[i];
            }

            return result;
        }

        consteval static std::meta::info get_component_store_type_info()
        {
            constexpr auto componentStoreTemplateInfo = get_component_store_template_info();
            constexpr auto args = get_component_store_template_args();
            const auto componentStoreTypeInfo = std::meta::substitute(componentStoreTemplateInfo, args);
            return componentStoreTypeInfo;
        }

        typename [:get_component_store_type_info():] m_componentStore;
#pragma endregion
    };
}
