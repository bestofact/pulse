//#include "app/ecs/module/scene.h"

#include "ecs/concepts/hasdependencyconcept.h"
#include "ecs/concepts/systemconcept.h"
#include "ecs/types/dataaccess.h"
#include "ecs/types/entity.h"
#include "ecs/types/systemhandle.h"
#include "ecs/utils/collectors.h"
#include "ecs/utils/generators.h"
#include "ecs/utils/invokers.h"
#include "foundation/types.h"


#include "ecs/concepts/entityconcept.h"
#include "ecs/types/archetypechunk.h"
#include "ecs/types/entitystore.h"
#include "ecs/types/archetypestore.h"
#include "ecs/types/world.h"
#include "ecs/types/outputhandle.h"
#include "ecs/types/systemchunk.h"
#include "ecs/types/systemstore.h"

#include "ecs/utils/meta.h"

#include "foundation/meta.h"
#include "foundation/types.h"

#include <meta>
#include <print>
#include <type_traits>


namespace pulse::ecs::module
{
    using Entity = pulse::ecs::Entity<100>;

    struct Config
    {
        u32 m_value = 0;
    };

    struct Name
    {
        u32 m_value = 0;
    };

    struct Size
    {
        u32 m_value = 0;
    };

    struct Result
    {
        using Output = int;
        u32 m_value = 0;
    };

    struct Result2
    {
        using Output = int;
        u32 m_value = 0;
    };

    struct Result3
    {
        using Output = int;
        u32 m_value = 0;
    };

    void Aoo(const Config&)
    {
        std::println("Aoo");
    }

    void Boo(const Config&)
    {
        std::println("Boo");
    }

    OutputHandle<Result> Coo(const Config&)
    {
        std::println("Coo");
        return {};
    }

    OutputHandle<Result2> Doo(const Config&)
    {
        OutputHandle<Result2> result;
        std::println("Doo");
        result.set_result(Result2{.m_value = 5});
        return result;
    }

    void Eoo(const Result& in_result)
    {
        std::println("Eoo");
    }

    OutputHandle<Result3> Foo(const Result2& in_result)
    {
        OutputHandle<Result3> out;
        std::println("Foo : {0}", in_result.m_value);
        out.set_result(Result3{in_result.m_value + 1});
        return out;
    }

    void Goo(const Result3& in_r)
    {
        std::println("Goo : {0}", in_r.m_value);
    }
}

int main()
{

    using World = [:pulse::ecs::generators::generate_default_world(^^pulse::ecs::module):];
    World w;

    w.add_component<pulse::ecs::module::Config, pulse::ecs::module::Name>(w.new_entity());
    w.add_component<pulse::ecs::module::Name>(w.new_entity());
    w.add_component<pulse::ecs::module::Size, pulse::ecs::module::Config>(w.new_entity());
    w.add_component<pulse::ecs::module::Size, pulse::ecs::module::Name>(w.new_entity());
    w.add_component<pulse::ecs::module::Config>(w.new_entity());
    w.apply_component_modifications();
    
    w.invoke_systems();

    return 0;
}
