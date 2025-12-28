#pragma once

#include "ecs/concepts/dataconcept.h"
#include "ecs/concepts/helpers.h"

namespace pulse::ecs::concepts
{
	template<typename T>
	concept Output =
	   pulse::ecs::concepts::Data<T>
	&& pulse::ecs::concepts::RequireOutputAlias<T>;
}