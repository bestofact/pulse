#include "app/ecs/module/scene.h"

#include <cstddef>
#include <meta>
#include <print>


int main()
{
    using namespace pulse::ecs::module;

    Scene s;


    
    // add entity
    // add component
    // remove component

    for(int i = 0; i < 3; ++i)
    {
        for(std::size_t e = 0; e < 10; ++e)
        {
            s.invoke_systems({e});
        }
    }
   
    
    return 0;
}
