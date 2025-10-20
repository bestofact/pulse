#include "app/ecs_test/humanoid/storage.h"
#include "app/ecs_test/humanoid/system/move.h"
#include "ecs/core/utility.h"

int main()
{

    constexpr auto ctx = std::meta::access_context::current();

    pulse::ecs::humanoid::storage::Type storage;
    pulse::ecs::construct_storage<ctx, ^^pulse::ecs::humanoid, pulse::ecs::humanoid::storage::Type, 1000>(storage);
    pulse::ecs::execute_storage_systems(storage);
    
    return 0;
}

