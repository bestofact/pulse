#pragma once

#include "app/ecs/module/components.h"
#include "app/ecs/module/systems.h"


#include "ecs/core/componentstorage.h"
#include "ecs/core/systemstorage.h"
#include "ecs/core/systemexecutorsequential.h"


namespace pulse::ecs::module::internal
{
    struct ComponentStorage;
    struct SystemStorage;
    struct SystemExecutor;
}

consteval
{
    pulse::ecs::define_component_storage<^^pulse::ecs::module, pulse::ecs::module::internal::ComponentStorage, 100>();

    pulse::ecs::define_system_storage<^^pulse::ecs::module, pulse::ecs::module::internal::SystemStorage>();
    pulse::ecs::define_system_executor_sequential<^^pulse::ecs::module::internal::SystemStorage, pulse::ecs::module::internal::SystemExecutor>();
}

