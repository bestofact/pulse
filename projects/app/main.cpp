#include "app/ecs/module/scene.h"

#include <cstddef>
#include <meta>
#include <print>

int main()
{
    using namespace pulse::ecs::module;

    Scene s;

    Scene::Entity e1{0};
    Scene::Entity e2{1};

    s.add_component<Name>(e1);
    s.add_component<Fame>(e1);
    s.add_component<Fame>(e2);


    s.invoke_systems();
    
    return 0;
}
