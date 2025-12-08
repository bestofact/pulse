//#include "app/ecs/module/scene.h"

#include <cstddef>
#include <meta>
#include <print>


#include "ecs/v3/utils/detail.h"
#include "ecs/v3/world/world.h"

namespace pulse::ecs::humanoid
{
    using Entity = pulse::ecs::Entity<std::size_t, 1000>;

    struct Position final
    {
        int x = 0;
    };
}

consteval
{
    constexpr auto a = pulse::ecs::utils::get_entity_capacity_member_info(^^pulse::ecs::humanoid);
    constexpr auto v = std::meta::extract<std::size_t>(a);
}

int main()
{
    using World = pulse::ecs::World<^^pulse::ecs::humanoid>;
    World humanoidEcs;
    const auto& pool = humanoidEcs.impl.m_componentStore.get_component_pool<pulse::ecs::humanoid::Position>();


    //using namespace pulse::ecs::module;

    //Scene s;

    //Scene::Entity e1{0};
    //Scene::Entity e2{1};

    //s.add_component<Name>(e1);
    //s.add_component<Fame>(e1);
    //s.add_component<Fame>(e2);

    //s.invoke_systems();

    return 0;
}
