/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Parallax Background Implementation
*/

#include "Parallax.hpp"
#include "shared.h"
#include <cmath>

Parallax::Parallax()
    : _gameWidth(GAME_WIDTH), _gameHeight(GAME_HEIGHT)
{
}

Parallax::~Parallax()
{
    Cleanup();
}

void Parallax::Init(void)
{
    this->_layers.clear();

    // Layer 1 : Fond le plus lointain (étoiles distantes) - vitesse la plus lente
    ParallaxLayer layer1;
    layer1.texture = LoadTexture("resources/sprites/parallax/layer1.png");
    layer1.scrollSpeed = 10.0f;  // Très lent - arrière-plan distant
    layer1.offsetX = 0.0f;
    layer1.scale = 1.0f;
    if (layer1.texture.id != 0)
        this->_layers.push_back(layer1);

    // Layer 2 : Étoiles moyennes
    ParallaxLayer layer2;
    layer2.texture = LoadTexture("resources/sprites/parallax/layer2.png");
    layer2.scrollSpeed = 25.0f;  // Un peu plus rapide
    layer2.offsetX = 0.0f;
    layer2.scale = 1.0f;
    if (layer2.texture.id != 0)
        this->_layers.push_back(layer2);

    // Layer 3 : Étoiles proches
    ParallaxLayer layer3;
    layer3.texture = LoadTexture("resources/sprites/parallax/layer3.png");
    layer3.scrollSpeed = 45.0f;  // Plus rapide
    layer3.offsetX = 0.0f;
    layer3.scale = 1.0f;
    if (layer3.texture.id != 0)
        this->_layers.push_back(layer3);

    // Layer 4 : Premier plan (poussière spatiale/étoiles très proches)
    ParallaxLayer layer4;
    layer4.texture = LoadTexture("resources/sprites/parallax/layer4.png");
    layer4.scrollSpeed = 70.0f;  // Le plus rapide - premier plan
    layer4.offsetX = 0.0f;
    layer4.scale = 1.0f;
    if (layer4.texture.id != 0)
        this->_layers.push_back(layer4);
}

void Parallax::Update(float deltaTime)
{
    for (ParallaxLayer& layer : this->_layers)
    {
        layer.offsetX += layer.scrollSpeed * deltaTime;

        // Calcul de la largeur scalée pour le wrap
        float textureWidth = static_cast<float>(layer.texture.width);
        float scaleX = this->_gameWidth / textureWidth;
        if (scaleX < 1.0f) scaleX = 1.0f;
        float scaledWidth = textureWidth * scaleX;

        // Reset quand le défilement dépasse la largeur scalée
        if (layer.offsetX >= scaledWidth)
        {
            layer.offsetX = 0.0f;
        }
    }
}

void Parallax::Draw(void)
{
    // D'abord, remplir l'écran avec du noir pour éviter le vert
    DrawRectangle(0, 0, static_cast<int>(this->_gameWidth), static_cast<int>(this->_gameHeight), BLACK);

    for (const ParallaxLayer& layer : this->_layers)
    {
        if (layer.texture.id == 0)
            continue;

        float textureWidth = static_cast<float>(layer.texture.width);
        float textureHeight = static_cast<float>(layer.texture.height);

        // Calculer le scale pour couvrir tout l'écran en hauteur et largeur
        float scaleX = this->_gameWidth / textureWidth;
        float scaleY = this->_gameHeight / textureHeight;
        
        // Utiliser le plus grand scale pour couvrir tout l'écran
        float scale = (scaleX > scaleY) ? scaleX : scaleY;
        
        // S'assurer que le scale est au minimum 1
        if (scale < 1.0f) scale = 1.0f;

        float scaledWidth = textureWidth * scale;
        float scaledHeight = textureHeight * scale;

        // Centrer verticalement si nécessaire
        float yOffset = (this->_gameHeight - scaledHeight) / 2.0f;
        if (yOffset > 0) yOffset = 0; // Ne pas décaler vers le bas

        // Position X avec scrolling
        float x1 = -fmod(layer.offsetX, scaledWidth);
        float x2 = x1 + scaledWidth;

        Rectangle sourceRec = { 0.0f, 0.0f, textureWidth, textureHeight };
        Vector2 origin = { 0.0f, 0.0f };

        // Dessiner la première copie
        Rectangle destRec1 = { x1, yOffset, scaledWidth, scaledHeight };
        DrawTexturePro(layer.texture, sourceRec, destRec1, origin, 0.0f, WHITE);

        // Dessiner la deuxième copie pour le scrolling infini
        Rectangle destRec2 = { x2, yOffset, scaledWidth, scaledHeight };
        DrawTexturePro(layer.texture, sourceRec, destRec2, origin, 0.0f, WHITE);

        // Troisième copie si nécessaire
        if (x2 + scaledWidth <= this->_gameWidth)
        {
            Rectangle destRec3 = { x2 + scaledWidth, yOffset, scaledWidth, scaledHeight };
            DrawTexturePro(layer.texture, sourceRec, destRec3, origin, 0.0f, WHITE);
        }
    }
}

void Parallax::Cleanup(void)
{
    for (ParallaxLayer& layer : this->_layers)
    {
        if (layer.texture.id != 0)
        {
            UnloadTexture(layer.texture);
            layer.texture.id = 0;
        }
    }
    this->_layers.clear();
}
