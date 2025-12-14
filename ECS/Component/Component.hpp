#pragma once
    #include <cstdint>
    #include <raylib.h>
    #include <array>

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

struct InputController{};

struct Tag{
    bool isPlayer;
};