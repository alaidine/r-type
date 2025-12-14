#include "Core.hpp"
#include "System/GravitySystem.hpp"
#include "System/RendererSystem.hpp"
#include "System/InputControllerSystem.hpp"
#include "Builder/Builder.hpp"
#include "Prefab.hpp"
#include <stdlib.h>
#include <time.h>

Core _core;

int main() {
    _core.Init();

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "TEST");
    
    MiniBuilder::RegisterComponentBuilder registerTest;
    registerTest.RegisterComponents<Position, Gravity, Velocity, Sprite, InputController, PlayerSprite, AnimationComponent>(_core);

    // auto gravitySystem = _core.RegisterSystem<GravitySystem>();
    // gravitySystem->order = 2;
    // auto renderSystem = _core.RegisterSystem<RendererSystem>();
    // renderSystem->order = 3;
    auto inputControllerSystem = _core.RegisterSystem<InputControllerSystem>();
    inputControllerSystem->order = 1;

    // Signature gravitySignature;
    // Signature RenderSignature;
    Signature inputControllerSignature;

    // MiniBuilder::SystemBuilder gravityBuilder(gravitySignature);
    // gravityBuilder.BuildSignature<GravitySystem, Gravity, Position>(_core);

    // MiniBuilder::SystemBuilder rendererBuilder(RenderSignature);
    // rendererBuilder.BuildSignature<RendererSystem, Position, Sprite>(_core);

    MiniBuilder::SystemBuilder inputControllerBuilder(inputControllerSignature);
    inputControllerBuilder.BuildSignature<InputControllerSystem, Position, InputController, PlayerSprite>(_core);

    Entity player = Prefab::MakePlayer(_core, (float)screenWidth/2, (float)screenHeight/2);
    
    srand(time(NULL));

    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        
        BeginDrawing();
        
        ClearBackground(LIGHTGRAY);
        DrawText("Test text", 10, 10, 20, BLACK);
        
        inputControllerSystem->Update();
        
        EndDrawing();
    }
    

    CloseWindow();

    return 0;
}