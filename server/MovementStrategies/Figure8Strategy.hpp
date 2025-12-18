/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Figure-8 (Lemniscate) Movement Strategy
*/

#ifndef FIGURE8STRATEGY_HPP_
#define FIGURE8STRATEGY_HPP_

#include "IMovementStrategy.hpp"

class Figure8Strategy : public IMovementStrategy {
    public:
        Figure8Strategy(float xSpeed, float width, float height, float speed, float startX, float centerY);
        Position update(const Position &currentPosition, float totalTime) override;

    private:
        float _xSpeed;
        float _width;
        float _height;
        float _speed;
        float _startX;
        float _centerY;
};

#endif // FIGURE8STRATEGY_HPP_
