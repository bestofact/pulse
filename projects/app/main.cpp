//#include "app/ecs/module/scene.h"

#include <cstddef>
#include <meta>
#include <print>


#include "ecs/v3/system/system.h"
#include "ecs/v3/utils/detail.h"
#include "ecs/v3/world/world.h"

namespace pulse::ecs::humanoid
{
    using Entity = pulse::ecs::Entity<std::size_t, 2000>;
    
    struct Position final
    {
        int x = 0;
        int y = 0;
    };

    struct Velocity final
    {
        int x = 0;
        int y = 0;
    };

    struct Input final
    {
        int x = 0;
        int y = 0;
    };

    struct Moving final
    {
        float speed = 0.0f;
    };

    void Foo(const Position& in_position)
    {

    }

/**
    template <typename T>
    struct output{ T m_data; };

    template <typename T>
    struct input{ T m_data; };

    template <typename T>
    struct add{ T m_data; };

    template <typename T>
    struct remove{ T m_data; };


    
    // This system outputs a Velocity value for each entity with input component.
    output<Velocity> UpdateVelocity(const Input& in_input)
    {
        Velocity v;
        v.x = in_input.x;
        v.y = in_input.y;

        return {v};
    }

    
    // This system reads velocity outputs (depends on all other systems that outputs a velocity)
    //  and returns another velocity output.
    output<Velocity> ClampVelocity(input<Velocity> in_velocityInput)
    {
      Velocity v;
      v.x = std::clamp(in_velocityInput.m_data.x, -2, 2);
      v.y = std::clamp(in_velocityInput.m_data.y, -2, 2);
      return {v};
    }

    void SetVelocity(Velocity& out_velocity, input<Velocity> in_velocityInput)
    {
        out_velocity.x = in_velocityInput.m_data.x;
        out_velocity.y = in_velocityInput.m_data.y;
    }

    void UpdatePosition(const Velocity& in_velocity, Position& out_position)
    {
        out_position.x += in_velocity.x;
        out_position.y += in_velocity.y;
    }

    void ToggleMoving(const Velocity& in_velocity, Components& out_components)
    {
        if(in_velocity.x == 0 && in_velocity.y == 0)
        {
          out_components.remove<Moving>();
        }
        else
        {
            const float speed = std::sqrtf(in_velocity.x * in_velocity.x + in_velocity.y * in_velocity.y);
            out_components.add<Moving>({.speed = speed});
        }
    }
*/
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
    const auto& pool = humanoidEcs.m_componentStore.get_component_pool<pulse::ecs::humanoid::Position>();
    


    /**
    auto s = pulse::ecs::System<decltype(pulse::ecs::humanoid::Foo), &pulse::ecs::humanoid::Foo>();
    s.invoke<
        decltype(humanoidEcs.m_componentStore)>(
            humanoidEcs.m_componentStore
    );*/
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
