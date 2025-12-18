#include "Prefab.hpp"

namespace Prefab {

Entity MakeEnemy(Core &_core, float posX, float posY) {
  Entity e = _core.CreateEntity();
  _core.AddComponent(e, Position{Vector2{posX, posY}});
  _core.AddComponent(e, Gravity{9.81});
  _core.AddComponent(e, Sprite{GREEN});
  _core.AddComponent(e, Velocity{4});

  return e;
}

Entity MakeMilssile(Core &_core) {
  Entity e = _core.CreateEntity();
  _core.AddComponent(
      e, AnimationComponent{
             Rectangle{0, 0, 0, 0},
             {Rectangle{0, 128, 25, 22}, Rectangle{25, 128, 31, 22},
              Rectangle{56, 128, 40, 22}, Rectangle{96, 128, 55, 22},
              Rectangle{151, 128, 72, 22}},
             0,
             0,
             8,
         });
  _core.AddComponent(e, Sprite{RED});
  _core.AddComponent(e, Position{Vector2{0, 0}});
  _core.AddComponent(e, Tag{false});
  _core.AddComponent(e, MissileTag{});
  return e;
}

Entity MakePlayer(Core &_core, float x, float y) {
  Entity e = _core.CreateEntity();
  PlayerSprite sprite;
  sprite.texture = LoadTexture("resources/sprites/player_r-9c_war-head.png");
  _core.AddComponent(e, Position{Vector2{x, y}});
  _core.AddComponent(e, InputController{});
  _core.AddComponent(e, sprite);
  _core.AddComponent(e, AnimationComponent{
                            Rectangle{0, 30, 32, 22},
                        });
  _core.AddComponent(e, Tag{true});
  _core.AddComponent(e, Sprite{WHITE});
  _core.AddComponent(e, playerCooldown{false});
  return e;
}
} // namespace Prefab