#include "app/modules/world.h"

#include "ecs/types/entity.h"
#include "ecs/types/outputhandle.h"
#include "ecs/utils/generators.h"
#include "foundation/types.h"

#include "entt.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstring>


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

    double start = now_ms();

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
    
    double end = now_ms();
    const double entityCreationMs = end - start;


    // Add random targets for the cache-killer system
    std::mt19937 rng(42);
    std::uniform_int_distribution<u32> dist(0, ENTITY_COUNT - 1);

    start = now_ms();
    for (auto e : entities)
    {
        world.add_component<Target>(e);
        world.get_component_mutable<Target>(e) = { entities[dist(rng)] };
    }
    end = now_ms();
    const double randomizeMs = end - start;

    start = now_ms();
    // Warmup
    for (int i = 0; i < 10; ++i)
        world.invoke_systems();

    end = now_ms();
    const double warmupMs = end - start;
    // -----------------------------------------------------------------
    // Timed run
    // -----------------------------------------------------------------
    start = now_ms();

    for (u32 i = 0; i < ITERATIONS; ++i)
        world.invoke_systems();

    end = now_ms();

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
    std::cout << "Creation MS      " << entityCreationMs << "\n";
    std::cout << "Randomize MS     " << randomizeMs << "\n";
    std::cout << "Warmup MS        " << warmupMs << "\n";
}



namespace pulse::ecs::bench_entt
{
    static constexpr std::size_t EntityCount = 1'000'000;

    // ---------------------------------------------------------------------
    // Components
    // ---------------------------------------------------------------------

    struct Position { f32 x = 0, y = 0, z = 0; };
    struct Velocity { f32 x = 0, y = 0, z = 0; };

    // EnTT entity handle for target
    struct Target { entt::entity id{ entt::null }; };

    // Sink component to prevent dead-code elimination
    struct BenchSink { f64 value = 0.0; };

    // ---------------------------------------------------------------------
    // Systems (function-shaped like your version)
    // ---------------------------------------------------------------------

    inline void Integrate(Position& p, const Velocity& v, BenchSink& sink)
    {
        p.x += v.x;
        p.y += v.y;
        p.z += v.z;

        sink.value += (f64)p.x;
    }

    inline void IntegrateAndMix(Position& p, const Velocity& v, BenchSink& sink)
    {
        p.x += v.x * 0.5f;
        p.y += v.y * 0.25f;
        p.z += v.z * 0.125f;

        sink.value += (f64)(p.x + p.y + p.z);
    }

    inline void RandomAccess(
        entt::registry& reg,
        const Target& t,
        const Position& my_pos,
        BenchSink& sink)
    {
        // Equivalent of: mod.value<Position>(t.id)
        // (Use try_get if you want to tolerate missing targets)
        const Position &other = reg.get<Position>(t.id);

        const f64 dx = (f64)other.x - (f64)my_pos.x;
        const f64 dy = (f64)other.y - (f64)my_pos.y;
        const f64 dz = (f64)other.z - (f64)my_pos.z;

        sink.value += dx*dx + dy*dy + dz*dz;
    }

    // ---------------------------------------------------------------------
    // Benchmark loops (what EnTT typically measures)
    // ---------------------------------------------------------------------

    inline void RunIntegrate(entt::registry& reg)
    {
        // Hot-path linear integration: (Position, Velocity, BenchSink)
        auto view = reg.view<Position, Velocity, BenchSink>();
        for (auto e : view) {
            auto &p = view.get<Position>(e);
            const auto &v = view.get<Velocity>(e);
            auto &sink = view.get<BenchSink>(e);
            Integrate(p, v, sink);
        }
    }

    inline void RunIntegrateAndMix(entt::registry& reg)
    {
        auto view = reg.view<Position, Velocity, BenchSink>();
        for (auto e : view) {
            auto &p = view.get<Position>(e);
            const auto &v = view.get<Velocity>(e);
            auto &sink = view.get<BenchSink>(e);
            IntegrateAndMix(p, v, sink);
        }
    }

    inline void RunRandomAccess(entt::registry& reg)
    {
        // Worst-case random access inside system: (Target, Position, BenchSink)
        auto view = reg.view<Target, Position, BenchSink>();
        for (auto e : view) {
            const auto &t = view.get<Target>(e);
            const auto &my_pos = view.get<Position>(e);
            auto &sink = view.get<BenchSink>(e);

            // You can decide whether to skip invalid targets (recommended for safety)
            if (t.id != entt::null && reg.valid(t.id) && reg.all_of<Position>(t.id)) {
                RandomAccess(reg, t, my_pos, sink);
            }
        }
    }

    // ---------------------------------------------------------------------
    // Setup (roughly your World construction step)
    // ---------------------------------------------------------------------

    inline entt::registry MakeWorld(std::uint32_t seed = 12345)
    {
        entt::registry reg;

        std::mt19937 rng(seed);
        std::uniform_real_distribution<f32> distVel(-1.0f, 1.0f);

        std::vector<entt::entity> entities;
        entities.reserve(EntityCount);

        // Create entities + baseline components
        for (std::size_t i = 0; i < EntityCount; ++i) {
            auto e = reg.create();
            entities.push_back(e);

            reg.emplace<Position>(e, Position{});
            reg.emplace<Velocity>(e, Velocity{ distVel(rng), distVel(rng), distVel(rng) });
            reg.emplace<BenchSink>(e, BenchSink{});
        }

        // Add Target to some or all entities; for worst-case, add to all.
        // If you want it closer to “some have target”, change this ratio.
        for (std::size_t i = 0; i < EntityCount; ++i) {
            // worst-case: everyone has a target
            const auto targetIndex = (i * 48271u + 1u) % EntityCount; // cheap LCG-ish scatter
            reg.emplace<Target>(entities[i], Target{ entities[targetIndex] });
        }

        return reg;
    }

      template <class Fn>
    inline double TimeItMs(Fn&& fn, int iters = 10)
    {
        using clock = std::chrono::high_resolution_clock;
        const auto start = clock::now();
        for (int i = 0; i < iters; ++i) fn();
        const auto end = clock::now();
        const std::chrono::duration<double, std::milli> dt = end - start;
        return dt.count() / (double)iters;
    }


} // namespace pulse::ecs::bench_entt



void benchamrk_entt()
{
     using namespace pulse::ecs::bench_entt;

    auto reg = MakeWorld();

    // Warmup (optional but nice to reduce first-touch noise)
    RunIntegrate(reg);
    RunIntegrateAndMix(reg);
    RunRandomAccess(reg);

    // Timed
    const double t1 = TimeItMs([&]{ RunIntegrate(reg); }, 10);
    const double t2 = TimeItMs([&]{ RunIntegrateAndMix(reg); }, 10);
    const double t3 = TimeItMs([&]{ RunRandomAccess(reg); }, 10);

    // Print something involving sinks to keep compilers honest-ish
    // (BenchSink exists on all entities)
    f64 checksum = 0.0;
    reg.view<pulse::ecs::bench_entt::BenchSink>().each([&](const pulse::ecs::bench_entt::BenchSink& s){ checksum += s.value; });

    std::cout
        << "Integrate:        " << t1 << " ms\n"
        << "IntegrateAndMix:  " << t2 << " ms\n"
        << "RandomAccess:     " << t3 << " ms\n"
        << "Checksum:         " << checksum << "\n";

}
