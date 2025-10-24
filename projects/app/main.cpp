#include "app/ecs/module/storage.h"
#include "app/ecs/module/system/initialize.h"
#include "ecs/core/systemexecution.h"
#include "ecs/core/systemexecutorsequential.h"
#include "ecs/core/utility.h"
#include "ecs/core/systemstorage.h"
#include <meta>
#include <type_traits>

struct storage;
struct executor;

consteval
{
    pulse::ecs::define_system_storage<^^pulse::ecs::module, storage>();
    pulse::ecs::define_system_executor_sequential<^^storage, executor>();
}

int main()
{
    //PULSE_ECS_CONSTRUCT_STORAGE(module_storage, module, 1000);
    //pulse::ecs::execute_storage_systems(module_storage);

    //pulse::ecs::module::storage::Type t;

    storage s;
    executor e;
    e.m_system_executor.execute_systems(s);

    return 0;
}