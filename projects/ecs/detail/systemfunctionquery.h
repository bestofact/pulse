#pragma once

#include <meta>

namespace pulse::ecs
{

/**
 * 
 * Types:
 * 	- ComponentMeta : Stores compile-time information about a component, registered to a scene.
 *  - ComponentWrapper : Actual data type that is used to store components.
 *  - ComponentMetaRegistry : Utility struct to access compile-time information about a component, 
 * 		using component type. Initialized with all substituded component meta types.
 *	
 * 	- SystemMeta : Stores compile-time information about a system, registered to a scene.
 *  - SystemWrapper : Actual data type that is used to store systems.
 *  - SystemMetaRegistry : Utility struct to access compile-time information about a system,
 * 		using system type. Initialized with all substituded system meta types.
 *  - SystemInvoker : Invokes of a system, using correct components passed as parameters.
 * 
 *  TODO(anil) find a good structure for querying entities.
 * 
 * 
 * 	Runtime Types
 *  - ComponentWrapper:
 * 		- bitset representing which entities actively has the component
 * 		- list of components.
 * 
 * 	- SystemWrapper:
 * 		- Function pointer to the system function.
 * 
 * 	
 * 
*/





	template<typename _FunctionType>
	struct SystemFunctionQuery
	{
	};
}