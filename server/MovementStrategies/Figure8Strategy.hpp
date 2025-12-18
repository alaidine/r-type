/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Figure-8 (Lemniscate) Movement Strategy
*/

#ifndef FIGURE8STRATEGY_HPP_
#define FIGURE8STRATEGY_HPP_

#include "IMovementStrategy.hpp"
#include <cmath>

class Figure8Strategy : public IMovementStrategy {
    public:
        Figure8Strategy(float xSpeed, float width, float height, float speed, float startX, float centerY)
            : _xSpeed(xSpeed), _width(width), _height(height), _speed(speed), _startX(startX), _centerY(centerY) {}

        Position update([[maybe_unused]] const Position &currentPosition, float totalTime) override
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

    private:
        float _xSpeed;
        float _width;
        float _height;
        float _speed;
        float _startX;
        float _centerY;
};

#endif // FIGURE8STRATEGY_HPP_
