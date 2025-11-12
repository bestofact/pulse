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
#include <random>


std::string random_string(std::size_t length, size_t seed)
{
    static const std::string chars =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::mt19937 gen(static_cast<unsigned>(seed));
    std::uniform_int_distribution<> dist(0, static_cast<int>(chars.size() - 1));

    std::string result;
    result.reserve(length);

    for (std::size_t i = 0; i < length; ++i)
        result.push_back(chars[dist(gen)]);

    return result;
}

bool random_bool(double chance)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::bernoulli_distribution dist(chance);
    return dist(gen);
}


/*
struct Debug;        
consteval
{
    using executor = pulse::ecs::SystemExecutor<
        pulse::ecs::module::Scene::SystemStorage, 
        pulse::ecs::module::Scene::ComponentStorage,
        pulse::ecs::SystemPointer<decltype(&pulse::ecs::module::ReadTest), &pulse::ecs::module::ReadTest>, 
        offsetof(pulse::ecs::module::Scene::SystemStorage, ReadTest)
    >;
    
    executor::SystemType a;
    using b = decltype(&pulse::ecs::module::ReadTest);
    using c = decltype(pulse::ecs::module::ReadTest);

    

    std::vector<std::meta::info> members;
    auto ps = std::meta::parameters_of(std::meta::remove_pointer(^^executor::SystemType));
    members.push_back(std::meta::data_member_spec(^^int, {.name = "m_dummy"}));
    for(auto p : ps)
    {
        members.push_back(std::meta::data_member_spec(
            p
        ));
    }

    std::meta::define_aggregate(^^Debug, members);
}
*/
int main()
{
   
    pulse::ecs::Scene2<^^pulse::ecs::module, 1000> s;
    auto& name = const_cast<pulse::ecs::module::Name&>(s.get_component<pulse::ecs::module::Name>(1));
    name.m_name = "Hello";
    s.execute_systems(1);
    /*
    pulse::ecs::module::Scene scene;

    bool a = scene.has_component<pulse::ecs::module::Name>(0);
    std::println("{}", a);

    scene.add_component<pulse::ecs::module::Name>(0);
    a = scene.has_component<pulse::ecs::module::Name>(0);
    std::println("{}", a);

    scene.remove_component<pulse::ecs::module::Name>(0);
    a = scene.has_component<pulse::ecs::module::Name>(0);
    std::println("{}", a);

    pulse::ecs::Entity e = 0;
    scene.execute_systems_for_entity(e);
    scene.debug_systems_for_entity(e);
*/
    return 0;
}
