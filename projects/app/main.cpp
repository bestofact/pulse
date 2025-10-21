#include "app/ecs/module/storage.h"
#include "ecs/core/utility.h"

int main()
{
    PULSE_ECS_CONSTRUCT_STORAGE(module_storage, module, 1000);
    pulse::ecs::execute_storage_systems(module_storage);

    return 0;
}