#pragma once
#include <array>
#include <cstdint>
#include <raylib.h>

using ComponentType = std::uint8_t;

const ComponentType MAX_COMPONENTS = 32;

struct Gravity {
  float force;
};

struct Position {
  Vector2 position;
};

struct Velocity {
  float speed;
};

struct Sprite {
  Color color;
};

struct PlayerSprite {
  Texture2D texture;
};

struct Collider {
  Rectangle rect;
};

struct AnimationComponent {
  Rectangle rect;
  std::array<Rectangle, 5> _animationRectangle;
  int _current_frame;
  int _frameCounter;
  int _frameSpeed;
};

struct InputController {};

struct Tag {
  bool isPlayer;
};

struct MissileTag {};

struct playerCooldown {
  bool canFire;
};

// Networked client component for multiplayer
struct NetworkedClient {
  uint32_t client_id;
  bool is_local;
};

// Tag to identify local player entity
struct LocalPlayerTag {};

// Tag to identify remote player entities
struct RemotePlayerTag {};
