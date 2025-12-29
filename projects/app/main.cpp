#pragma once
#include "app/modules/world.h"


#include "ecs/types/entity.h"
#include "ecs/types/outputhandle.h"

#include "ecs/utils/generators.h"
#include "foundation/types.h"



#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>



namespace pulse::ecs::bench
{
    // -------------------------------------------------------------------------
    // 1. CONFIGURATION
    // -------------------------------------------------------------------------
    using Entity = pulse::ecs::Entity<65'000>;

    // -------------------------------------------------------------------------
    // 2. COMPONENTS
    // -------------------------------------------------------------------------
    struct Position { f32 x = 0; f32 y = 0; f32 z = 0; };
    struct Velocity { f32 x = 0; f32 y = 0; f32 z = 0; };
    
    // Standard benchmark "padding" component. 
    // We add this to entities to test if your ECS can handle skipping 
    // irrelevant data without cache misses (the "AoS vs SoA" test).
    struct DataPadding { f32 pad[8]; }; 

    // A resource component for DeltaTime (assuming your ECS supports singletons, 
    // otherwise we hardcode dt in the system).
    struct FrameConfig { f32 dt = 0.016f; };

    // -------------------------------------------------------------------------
    // 3. SYSTEM IO (The "Glue")
    // -------------------------------------------------------------------------
    // The modifier lets us write back to the entity
    using Modifier = [:pulse::ecs::generators::generate_default_component_modifier(^^pulse::ecs::bench):];

    // The intermediate data for the pipeline
    struct MovementResult
    {
        using Output = int; // Preserving your framework's required typedef
        f32 x, y, z;
    };

    // -------------------------------------------------------------------------
    // 4. SYSTEMS
    // -------------------------------------------------------------------------
    
    // SYSTEM A: THE CALCULATOR
    // Reads Position and Velocity, computes the next step.
    // Does NOT mutate directly (Pure Functional approach).
    OutputHandle<MovementResult> ComputeMovement(const Position& pos, const Velocity& vel)
    {
        OutputHandle<MovementResult> out;
        
        // Standard "euler integration" step
        const f32 dt = 0.016f; // Fixed 60FPS step for deterministic benchmarking
        
        MovementResult result;
        result.x = pos.x + (vel.x * dt);
        result.y = pos.y + (vel.y * dt);
        result.z = pos.z + (vel.z * dt);

        out.set_result(std::move(result));
        return out;
    }

    // SYSTEM B: THE APPLIER
    // Takes the calculation result and writes it back to the ECS storage.
    void ApplyMovement(const MovementResult& result, Modifier modifier)
    {
        // We construct a new Position. 
        // Note: If your framework supports partial updates, that would be preferred,
        // but replacing the component is the standard stress test for "Write Bandwidth".
        modifier.set(Position{ result.x, result.y, result.z });
    }

    // -------------------------------------------------------------------------
    // 5. WORLD GENERATION
    // -------------------------------------------------------------------------
    // This generates the graph: [Position, Velocity] -> ComputeMovement -> [MovementResult] -> ApplyMovement -> [Modifier]
    using World = [:pulse::ecs::generators::generate_default_world(^^pulse::ecs::bench):];
}

// [Assuming your framework and the 'pulse::ecs::bench::movement' namespace are included above]
void RunBenchmark()
{
    using namespace pulse;
    using namespace pulse::ecs::bench;

    using Clock = std::chrono::high_resolution_clock;
    using Ms = std::chrono::duration<double, std::milli>;

    // ---------------------------------------------------------
    // 1. SETUP
    // ---------------------------------------------------------
    const int ENTITY_COUNT = 65'000; // Power of 2, fits in <100'000>
    const int ITERATIONS = 2'000;    // Enough frames to smooth out noise
    
    std::cout << "--- BENCHMARK STARTING ---\n";
    std::cout << "Setup: " << ENTITY_COUNT << " Entities | " << ITERATIONS << " Frames\n";

    World world;

    // Populate World
    // We add Position & Velocity to match the 'ComputeMovement' system requirements
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        auto e = world.new_entity();
        // 1. Add Data (to create the "Archetype")
        world.add_component<Position>(e);
        world.add_component<Velocity>(e);
        world.get_component_mutable<Velocity>(e) = {1.0f, 0.5f, 0};
        
        // 2. Add Padding to half the entities
        // This forces the ECS to deal with fragmented memory or multiple tables/archetypes,
        // which is a critical part of the "Industry Standard" test.
        if (i % 2 == 0) {
            world.add_component<DataPadding>(e);
        }
    }

    // 2. FRAGMENTATION: "Kill" every even entity
    // In your deferred bitset system, this should just flip a bit.
    // Ideally, use whatever function removes the 'Position' or 'Velocity' 
    // component to disqualify them from the system.
    for (u64 i = 0; i < world.get_entity_capacity(); ++i) {
        if (i % 2 == 0) {
            // Remove component to remove from system processing
            world.remove_component<Velocity>(pulse::ecs::bench::Entity(i)); 
        }
    }

    // ---------------------------------------------------------
    // 2. WARMUP
    // ---------------------------------------------------------
    // Run systems once to initialize internal caches/instruction caches
    // and avoid measuring the first-frame cold start penalty.
    world.invoke_systems(); 

    // ---------------------------------------------------------
    // 3. MEASUREMENT
    // ---------------------------------------------------------
    auto start_time = Clock::now();

    for (int i = 0; i < ITERATIONS; ++i) {
        world.invoke_systems();
    }

    auto end_time = Clock::now();

    // ---------------------------------------------------------
    // 4. REPORTING
    // ---------------------------------------------------------
    Ms total_duration = end_time - start_time;
    double avg_ms = total_duration.count() / ITERATIONS;
    double fps_equiv = 1000.0 / avg_ms;

    std::cout << "\n--- RESULTS ---\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Total Time:   " << total_duration.count() << " ms\n";
    std::cout << "Avg Frame:    " << avg_ms << " ms\n";
    std::cout << "Throughput:   " << fps_equiv << " FPS (Simulated)\n";
    std::cout << "Per Entity:   " << (avg_ms * 1'000'000 / ENTITY_COUNT) << " ns\n";
    //std::cout << "Checksum: " << total_x << " (Prevents optimization)\n";
    
    std::cout << "----------------\n";
}


static void run_modules()
{
    using namespace pulse;
    using namespace pulse::ecs::modules;

    World world;

    for(u64 i = 0; i < 100; i++)
    {
        const Entity entity = world.new_entity();
        world.add_component<Name>(entity);
        world.get_component_mutable<Name>(entity).m_name = std::format("Module {0}", entity.get_index());
    }

    for(u64 i = 0; i < 100; i++)
    {
        world.invoke_systems();
    }
}

int main()
{
    RunBenchmark();
    return 0;
}
