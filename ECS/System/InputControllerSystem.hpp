#pragma once
    #include "../Core.hpp"
    #include "System.hpp"
    #include "../Component/Component.hpp"
    #include "../Prefab.hpp"

    #ifndef MAX
    #define MAX(a, b) (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef MIN
    #define MIN(a, b) (((a) < (b)) ? (a) : (b))
    #endif

    #define GAME_WIDTH 800
    #define GAME_HEIGHT 600

    #define TARGET_FPS 100

extern Core _core;

void DrawMissiles(Position& pos, AnimationComponent& anim, Sprite& sprite, Texture2D& player) {
   
    anim._frameCounter++;

    if (anim._frameCounter >= (TARGET_FPS / anim._frameSpeed))
    {
        anim._frameCounter = 0;
        anim._current_frame++;
        if (anim._current_frame > 4) anim._current_frame = 0;
        anim.rect.x = anim._animationRectangle[anim._current_frame].x;
        anim.rect.y = anim._animationRectangle[anim._current_frame].y;
        anim.rect.height = anim._animationRectangle[anim._current_frame].height;
        anim.rect.width = anim._animationRectangle[anim._current_frame].width;
    }

    pos.position.x += 5;

    float frameWidth = anim.rect.width;
    float frameHeight = anim.rect.height;
    Rectangle sourceRec = { anim.rect.x, anim.rect.y, frameWidth, frameHeight };
    Rectangle rec = { (float)pos.position.x, (float)pos.position.y, frameWidth * 2.0f, frameHeight * 2.0f };
    Rectangle destRec = { (float)pos.position.x, (float)pos.position.y, frameWidth * 2.0f, frameHeight * 2.0f };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(player, sourceRec, destRec, origin, 0.0f, sprite.color);

}

void DrawPlayer(Position& pos, Texture2D& playerTexture) {
    float frameWidth = 32;
    float frameHeight = 22.0f;
    Rectangle sourceRec = { 0.0f, 30.0f, frameWidth, frameHeight };
    Rectangle rec = { pos.position.x, pos.position.y, frameWidth * 2.0f, frameHeight * 2.0f };
    Rectangle destRec = { pos.position.x, pos.position.y, frameWidth * 2.0f, frameHeight * 2.0f};
    Vector2 origin = { 0.0f, 0.0f };

    // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
    // source_rect defines the part of the texture we use for drawing
    // dest_rect defines the rectangle where our texture part will fit (scaling it to fit)
    // origin defines the point of the texture used as reference for rotation and scaling
    // rotation defines the texture rotation (using origin as rotation point)
    DrawTexturePro(playerTexture, sourceRec, destRec, origin, 0.0f, WHITE);
    // DrawRectangleLinesEx(rec, 3, DARKBROWN);
}


void fire(Vector2& missilePos, Vector2& palyerPos, AnimationComponent& missileAnim)
{
    missilePos.x = palyerPos.x;
    missilePos.y = palyerPos.y;
    missileAnim.rect.x= missileAnim._animationRectangle[missileAnim._current_frame].x;
    missileAnim.rect.y= missileAnim._animationRectangle[missileAnim._current_frame].y;
    missileAnim.rect.height = missileAnim._animationRectangle[missileAnim._current_frame].height;
    missileAnim.rect.width = missileAnim._animationRectangle[missileAnim._current_frame].width;
}

class InputControllerSystem : public System {
    public:
        void Update() override {
            bool fireKeyPressed = false;
            for (auto& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& input = _core.GetComponent<InputController>(entity);
                auto& playertexture = _core.GetComponent<PlayerSprite>(entity);
                if (IsKeyDown(KEY_SPACE) && !fireKeyPressed) {
                    fireKeyPressed = true;
                    Entity missile = Prefab::MakeMilssile(_core);
                    auto& missilePos = _core.GetComponent<Position>(missile);
                    auto&  anim = _core.GetComponent<AnimationComponent>(missile);
                    auto& sprite = _core.GetComponent<Sprite>(missile);
                    fire(missilePos.position, pos.position, anim);
                    DrawMissiles(missilePos, anim, sprite, playertexture.texture);
                }
                if (IsKeyUp(KEY_SPACE))
                    fireKeyPressed = true;
                
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
                    pos.position.y = MAX(0, pos.position.y - 5);
                else if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
                    pos.position.y = MIN(GAME_HEIGHT - 50, pos.position.y + 5);
                
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                    pos.position.x = MAX(0, pos.position.x - 5);
                else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                    pos.position.x = MIN(GAME_WIDTH - 50, pos.position.x + 5);
                DrawPlayer(pos, playertexture.texture);
            }
        }
};