/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Parallax Background Header
*/

#ifndef PARALLAX_HPP_
#define PARALLAX_HPP_

#include "raylib.h"
#include <vector>

struct ParallaxLayer {
    Texture2D texture;
    float scrollSpeed;
    float offsetX;
    float scale;
};

class Parallax {
    public:
        Parallax();
        ~Parallax();

        void Init(void);
        void Update(float deltaTime);
        void Draw(void);
        void Cleanup(void);

    private:
        std::vector<ParallaxLayer> _layers;
        float _gameWidth;
        float _gameHeight;
};

#endif // PARALLAX_HPP_
