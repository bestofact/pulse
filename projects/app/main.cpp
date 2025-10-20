#include "app/ecs/module/storage.h"

#include "ecs/core/utility.h"

int main()
{
    constexpr auto ctx = std::meta::access_context::current();

    pulse::ecs::module::storage::Type storage;
    pulse::ecs::construct_storage<ctx, ^^pulse::ecs::module, decltype(storage), 1000>(storage);
    pulse::ecs::execute_storage_systems(storage);

    return 0;
}