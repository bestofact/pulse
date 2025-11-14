#include "app/ecs/module/scene.h"
#include "app/ecs/module/system/readertest.h"
#include "ecs/core/access.h"
#include "ecs/entity/entity.h"
#include "ecs/scene/scene.h"
#include "ecs/system/systemexecutioninfo.h"
#include "ecs/system/systemstorage.h"


#include <cstddef>
#include <meta>
#include <print>


int main()
{
    using namespace pulse::ecs::module;

    pulse::ecs::module::Scene s;
    
    // add entity
    // add component
    // remove component

    for(std::size_t e = 0; e < 10; ++e)
    {
        s.execute_systems(e);
    }
    
    return 0;
}
