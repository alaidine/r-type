#include "Builder/Builder.hpp"
#include "Core.hpp"
#include "Prefab.hpp"
#include "System/GravitySystem.hpp"
#include "System/InputControllerSystem.hpp"
#include "System/MissileSystem.hpp"
#include "System/RendererSystem.hpp"
#include <stdlib.h>
#include <time.h>

Core _core;

int foo() {
  _core.Init();

  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "TEST");

  MiniBuilder::RegisterComponentBuilder registerTest;
  registerTest.RegisterComponents<
      Position, Gravity, Velocity, Sprite, InputController, PlayerSprite,
      AnimationComponent, Tag, MissileTag, playerCooldown>(_core);

  auto renderSystem = _core.RegisterSystem<RendererSystem>();
  renderSystem->order = 3;
  auto inputControllerSystem = _core.RegisterSystem<InputControllerSystem>();
  inputControllerSystem->order = 1;
  auto missileSystem = _core.RegisterSystem<MissileSystem>();
  missileSystem->order = 2;

  // Signature gravitySignature;
  Signature RenderSignature;
  Signature inputControllerSignature;
  Signature missileSystemSignature;

  MiniBuilder::SystemBuilder rendererBuilder(RenderSignature);
  rendererBuilder
      .BuildSignature<RendererSystem, Position, Sprite, AnimationComponent>(
          _core);

  MiniBuilder::SystemBuilder inputControllerBuilder(inputControllerSignature);
  inputControllerBuilder.BuildSignature<InputControllerSystem, Position,
                                        InputController, PlayerSprite>(_core);

  MiniBuilder::SystemBuilder missileSystemBuilder(missileSystemSignature);
  missileSystemBuilder
      .BuildSignature<MissileSystem, Position, AnimationComponent, MissileTag>(
          _core);

  Entity player = Prefab::MakePlayer(_core, (float)screenWidth / 2,
                                     (float)screenHeight / 2);

  SetTargetFPS(TARGET_FPS);

  while (!WindowShouldClose()) {

    BeginDrawing();

    ClearBackground(LIGHTGRAY);
    DrawText("Test text", 10, 10, 20, BLACK);

    _core.UpdateAllSystem();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
