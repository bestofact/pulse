#include "app/ecs/module/scene.h"


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

int main()
{
    pulse::ecs::module::Scene scene;
    
    bool a = scene.has_component<pulse::ecs::module::Name>(0);
    std::println("{}", a);

    scene.add_component<pulse::ecs::module::Name>(0);
    a = scene.has_component<pulse::ecs::module::Name>(0);
    std::println("{}", a);

    scene.remove_component<pulse::ecs::module::Name>(0);
    a = scene.has_component<pulse::ecs::module::Name>(0);
    std::println("{}", a);



    return 0;
}
