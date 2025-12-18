#pragma once
#include "../Core.hpp"
#include <raylib.h>

extern Core _core;

// class PlayerRender

class RendererSystem : public System {
private:
  Texture2D texture;
  Rectangle sourcerec;
  Rectangle destrec;
  Rectangle rec;
  Vector2 origin = {0.0f, 0.0f};

public:
  void Update() override {

    for (auto const &entity : _entities) {
      auto &pos = _core.GetComponent<Position>(entity);
      auto &sprite = _core.GetComponent<Sprite>(entity);
      auto &anim = _core.GetComponent<AnimationComponent>(entity);
      auto &tag = _core.GetComponent<Tag>(entity);
      if (tag.isPlayer) {
        auto &playerTexture = _core.GetComponent<PlayerSprite>(entity);
        texture = playerTexture.texture;
      }
      sourcerec = {anim.rect.x, anim.rect.y, anim.rect.width, anim.rect.height};
      rec = {pos.position.x, pos.position.y, anim.rect.width * 2.0f,
             anim.rect.height * 2.0f};
      destrec = {pos.position.x, pos.position.y, anim.rect.width * 2.0f,
                 anim.rect.height * 2.0f};
      DrawTexturePro(texture, sourcerec, destrec, origin, 0.0f, sprite.color);
    }
  }
};