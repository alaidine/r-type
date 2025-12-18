/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Figure-8 (Lemniscate) Movement Strategy Implementation
*/

#include "Figure8Strategy.hpp"
#include <cmath>

Figure8Strategy::Figure8Strategy(float xSpeed, float width, float height, float speed, float startX, float centerY)
    : _xSpeed(xSpeed), _width(width), _height(height), _speed(speed), _startX(startX), _centerY(centerY)
{
}

Position Figure8Strategy::update([[maybe_unused]] const Position &currentPosition, float totalTime)
{
    Position newPosition;
    float t = this->_speed * totalTime;
    float denominator = 1.0f + std::sin(t) * std::sin(t);
    float localX = this->_width * std::cos(t) / denominator;
    float localY = this->_height * std::sin(t) * std::cos(t) / denominator;
    newPosition.x = this->_startX - (this->_xSpeed * totalTime) + localX;
    newPosition.y = this->_centerY + localY;
    return newPosition;
}
