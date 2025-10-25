#include "app/ecs/module/storage.h"
#include "app/ecs/module/system/initialize.h"
#include "ecs/core/componentstorage.h"
#include "ecs/core/systemexecution.h"
#include "ecs/core/systemexecutorsequential.h"
#include "ecs/core/utility.h"
#include "ecs/core/systemstorage.h"
#include <meta>
#include <type_traits>

struct storage;
struct executor;
struct components;

consteval
{
    pulse::ecs::define_system_storage<^^pulse::ecs::module, storage>();
    pulse::ecs::define_system_executor_sequential<^^storage, executor>();

    pulse::ecs::define_component_storage<^^pulse::ecs::module, components, 100>();
}

int main()
{
    //PULSE_ECS_CONSTRUCT_STORAGE(module_storage, module, 1000);
    //pulse::ecs::execute_storage_systems(module_storage);

    //pulse::ecs::module::storage::Type t;

    storage s;
    executor e;
    e.m_system_executor.execute_systems(s);

    components c;
    c.Name[0].m_name = "Module";

    return 0;
}