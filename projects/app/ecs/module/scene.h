#pragma once

#include "app/ecs/module/components.h"
#include "app/ecs/module/systems.h"

#include "ecs/scene/scene.h"

#include "ecs/v3/entity/entitycapacity.h"

namespace pulse::ecs::module
{
    static constexpr auto s_entityCapacity = pulse::ecs::EntityCapacity(1000);

    using Scene = pulse::ecs::Scene<^^pulse::ecs::module, s_entityCapacity>;
}
