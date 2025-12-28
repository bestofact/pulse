#pragma once

#include "ecs/concepts/dataconcept.h"
#include "ecs/concepts/entityconcept.h"
#include "foundation/meta.h"

#include <meta>
#include <type_traits>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept Component = 
	   pulse::ecs::concepts::Data<T>
	&& pulse::ecs::concepts::RequireEcsGrandParentNamespace<T>
	&& pulse::ecs::concepts::ForbidOutputAlias<T>
	&& pulse::meta::any_neioughbour_satisfies_concept(^^T, ^^pulse::ecs::concepts::Entity, true)
	&& !std::is_empty_v<T>;
}