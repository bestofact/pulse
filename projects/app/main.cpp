#include "app/ecs/module/storage.h"

int main()
{

    pulse::ecs::module::internal::ComponentStorage component_storage;
    pulse::ecs::module::internal::SystemStorage system_storage;
    pulse::ecs::module::internal::SystemExecutor system_executor;
    
    system_executor.m_system_executor.execute_systems(system_storage);
    component_storage.Name[0].m_name = "Module";

    return 0;
}