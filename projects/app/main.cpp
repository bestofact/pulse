#pragma once
#include "app/modules/world.h"

#include "ecs/types/entity.h"
#include "ecs/utils/generators.h"
#include "foundation/types.h"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstring>

#pragma once
#include "ecs/types/entity.h"
#include "ecs/types/outputhandle.h"
#include "ecs/utils/generators.h"
#include "foundation/types.h"

namespace pulse::ecs::bench
{
    using Entity = pulse::ecs::Entity<1'000'000>;
    // ---------------------------------------------------------------------
    // Components
    // ---------------------------------------------------------------------

    struct Position
    {
        f32 x = 0, y = 0, z = 0;
    };

    struct Velocity
    {
        f32 x = 0, y = 0, z = 0;
    };

    struct Target
    {
        Entity id;
    };

    // Sink component to prevent dead-code elimination
    struct BenchSink
    {
        f64 value = 0.0;
    };

    // ---------------------------------------------------------------------
    // Glue: Modifier (AFTER components, BEFORE systems)
    // ---------------------------------------------------------------------

    using Modifier = [:pulse::ecs::generators::generate_default_component_modifier(^^pulse::ecs::bench):];

    // ---------------------------------------------------------------------
    // Systems
    // ---------------------------------------------------------------------

    // 1) Hot-path linear integration
    void Integrate(Position& p, const Velocity& v, BenchSink& sink)
    {
        p.x += v.x;
        p.y += v.y;
        p.z += v.z;

        sink.value += (f64)p.x;
    }

    // 2) Multi-component join
    void IntegrateAndMix(Position& p, const Velocity& v, BenchSink& sink)
    {
        p.x += v.x * 0.5f;
        p.y += v.y * 0.25f;
        p.z += v.z * 0.125f;

        sink.value += (f64)(p.x + p.y + p.z);
    }

    // 3) Worst-case random access INSIDE ECS system
    void RandomAccess(
        const Target& t,
        const Position& my_pos,
        Modifier mod,
        BenchSink& sink)
    {
        const Position& other = mod.value<Position>(t.id);

        const f64 dx = (f64)other.x - (f64)my_pos.x;
        const f64 dy = (f64)other.y - (f64)my_pos.y;
        const f64 dz = (f64)other.z - (f64)my_pos.z;

        sink.value += dx*dx + dy*dy + dz*dz;
    }

    // ---------------------------------------------------------------------
    // Glue: World (LAST thing in namespace)
    // ---------------------------------------------------------------------

    using World = [:pulse::ecs::generators::generate_default_world(^^pulse::ecs::bench):];
}

using namespace pulse;
using namespace pulse::ecs::bench;

static double now_ms()
{
    using Clock = std::chrono::high_resolution_clock;
    static auto t0 = Clock::now();
    return std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
}

void RunEcsSystemBenchmark()
{
    constexpr u32 ENTITY_COUNT = 1'000'000;
    constexpr u32 ITERATIONS   = 300;

    World world;
    std::vector<Entity> entities;
    entities.reserve(ENTITY_COUNT);

    // -----------------------------------------------------------------
    // Entity creation
    // -----------------------------------------------------------------
    for (u32 i = 0; i < ENTITY_COUNT; ++i)
    {
        auto e = world.new_entity();

        world.add_component<Position>(e);
        world.add_component<Velocity>(e);
        world.add_component<BenchSink>(e);

        world.get_component_mutable<Position>(e) = { (f32)i, 0.0f, 0.0f };
        world.get_component_mutable<Velocity>(e) = { 1.0f, 0.5f, 0.25f };

        entities.push_back(e);
    }

    // Add random targets for the cache-killer system
    std::mt19937 rng(42);
    std::uniform_int_distribution<u32> dist(0, ENTITY_COUNT - 1);

    for (auto e : entities)
    {
        world.add_component<Target>(e);
        world.get_component_mutable<Target>(e) = { entities[dist(rng)] };
    }

    // Warmup
    for (int i = 0; i < 10; ++i)
        world.invoke_systems();

    // -----------------------------------------------------------------
    // Timed run
    // -----------------------------------------------------------------
    const double start = now_ms();

    for (u32 i = 0; i < ITERATIONS; ++i)
        world.invoke_systems();

    const double end = now_ms();

    // -----------------------------------------------------------------
    // Reporting
    // -----------------------------------------------------------------
    const double total_ms = end - start;
    const double avg_ms   = total_ms / ITERATIONS;
    const double ns_per_entity =
        (avg_ms * 1e6) / (double)ENTITY_COUNT;

    // checksum
    f64 checksum = 0.0;
    for (auto e : entities)
        checksum += world.get_component<BenchSink>(e).value;

    std::cout << "\n--- ECS SYSTEM BENCHMARK ---\n";
    std::cout << "Entities:        " << ENTITY_COUNT << "\n";
    std::cout << "Iterations:      " << ITERATIONS << "\n";
    std::cout << "Avg frame:       " << avg_ms << " ms\n";
    std::cout << "ns/entity-step:  " << ns_per_entity << "\n";
    std::cout << "Checksum:        " << checksum << "\n";
}

int main()
{
    RunEcsSystemBenchmark();
    return 1;
}
