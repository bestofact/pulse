#pragma once

#include "foundation/meta.h"
#include <meta>

namespace pulse::ecs::concepts
{
	template<typename T>
	concept RequireOutputAlias = 
	requires 
	{
		typename T::Output;
	};

	template<typename T>
	concept ForbidOutputAlias = !RequireOutputAlias<T>;

	template<typename T>
	concept RequireEcsAnyParentNamespace = pulse::meta::is_child_of(^^T, ^^pulse::ecs, true);

	template<typename T>
	concept RequireEcsGrandParentNamespace = pulse::meta::is_child_of(^^T, ^^pulse::ecs, true, 1);
}