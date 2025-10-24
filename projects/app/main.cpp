#include "app/ecs/module/storage.h"
#include "app/ecs/module/system/initialize.h"
#include "ecs/core/utility.h"
#include "ecs/core/systemstorage.h"
#include <meta>
#include <type_traits>

struct test;

consteval
{
    constexpr auto ctx = std::meta::access_context::current();
    constexpr auto ns = ^^pulse::ecs::module;
    constexpr auto systems = pulse::ecs::SystemCollector<ctx, ns, 1000>();

    std::vector<std::meta::info> v;
    template for(constexpr auto system : systems.m_data)
    {
        if(system != std::meta::info{})
        {
            v.push_back(
                std::meta::data_member_spec(
                    std::meta::substitute(^^pulse::ecs::SystemContainer, { std::meta::type_of(system) }),
                    {
                        .name = std::meta::identifier_of(system)
                    }
                )
            );
        }
    }

    std::meta::define_aggregate(^^test, v);
}

struct storage;
struct executor;

consteval
{
    pulse::ecs::define_system_storage<^^pulse::ecs::module, storage>();
    pulse::ecs::define_system_executor<^^storage, executor>();
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