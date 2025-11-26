#include "Core.hpp"
#include "System/GravitySystem.hpp"
#include "Builder/Builder.hpp"

Core _core;

int main() {
    _core.Init();
    
    // _core.RegisterComponent<Position>();
    // _core.RegisterComponent<Gravity>();
    // _core.RegisterComponent<Velocity>();
    MiniBuilder::RegisterComponentBuilder registerTest;
    registerTest.RegisterComponents<Position, Gravity, Velocity>();


    auto gravitySystem = _core.RegisterSystem<GravitySystem>();

    Signature gravitySignature;

    // gravitySignature.set(_core.GetComponentType<Gravity>(), true);
    // gravitySignature.set(_core.GetComponentType<Position>(), true);

    // _core.SetSystemSignature<GravitySystem>(gravitySignature);
    MiniBuilder::SystemBuilder sysBuilder(gravitySignature);
    sysBuilder.BuildSignature<GravitySystem, Gravity, Position>();

    // Entity player = _core.CreateEntity();
    // _core.AddComponent(player, Position{4.0f, 2.0f});
    // _core.AddComponent(player, Gravity{-9.81f});

    // Entity ship = _core.CreateEntity();
    // _core.AddComponent(ship, Position{4.0f, 2.0f});

    Entity player = _core.CreateEntity();
    MiniBuilder::EntityBuilder playerBuilder(player);
    playerBuilder.BuildEntity(Position{8.0f, 8.0f}, Gravity{8.0f});

    Entity enemy = _core.CreateEntity();
    MiniBuilder::EntityBuilder enemyBuilder(enemy);
    enemyBuilder.BuildEntity(Position{4.0f, 2.0f}, Gravity{8.0f});

    gravitySystem->Update();

    return 0;
}