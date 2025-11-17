#include "app/ecs/module/scene.h"

#include <cstddef>
#include <meta>
#include <print>


int main()
{
    using namespace pulse::ecs::module;

    Scene s;

    constexpr bool debugStructrualOperations = true;
    if(debugStructrualOperations)
    {
        Scene::Entity e1{0};

        s.add_component<Fame>(e1);
        std::println("Name : {0}, Fame : {1}", s.has_component<Name>(e1), s.has_component<Fame>(e1));
        s.remove_component<Fame>(e1);
        s.add_component<Name>(e1);
        std::println("Name : {0}, Fame : {1}", s.has_component<Name>(e1), s.has_component<Fame>(e1));
        s.remove_component<Name>(e1);
        std::println("Name : {0}, Fame : {1}", s.has_component<Name>(e1), s.has_component<Fame>(e1));
    }

    
    // add entity
    // add component
    // remove component

    for(int i = 0; i < 3; ++i)
    {
        for(std::size_t e = 0; e < 10; ++e)
        {
            //s.invoke_systems({e});
        }
    }
   
    
    return 0;
}
