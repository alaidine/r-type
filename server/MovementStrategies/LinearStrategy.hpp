/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Linear Movement Strategy
*/

#ifndef LINEARSTRATEGY_HPP_
#define LINEARSTRATEGY_HPP_

#include "IMovementStrategy.hpp"

class LinearStrategy : public IMovementStrategy {
    public:
        LinearStrategy(float xSpeed, float ySpeed, float startX, float startY)
            : _xSpeed(xSpeed), _ySpeed(ySpeed), _startX(startX), _startY(startY) {}

        Position update([[maybe_unused]] const Position &currentPosition, float totalTime) override
        {
            Position newPosition;
            newPosition.x = this->_startX - (this->_xSpeed * totalTime);
            newPosition.y = this->_startY + (this->_ySpeed * totalTime);
            return newPosition;
        }

    private:
        float _xSpeed;
        float _ySpeed;
        float _startX;
        float _startY;
};

#endif // LINEARSTRATEGY_HPP_
