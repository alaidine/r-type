/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Circular Movement Strategy - Avance en faisant des cercles
*/

#ifndef CIRCULARSTRATEGY_HPP_
#define CIRCULARSTRATEGY_HPP_

#include "IMovementStrategy.hpp"
#include <cmath>

class CircularStrategy : public IMovementStrategy {
    public:
        CircularStrategy(float xSpeed, float radius, float angularSpeed, float startX, float centerY)
            : _xSpeed(xSpeed), _radius(radius), _angularSpeed(angularSpeed), _startX(startX), _centerY(centerY) {}

        Position update([[maybe_unused]] const Position &currentPosition, float totalTime) override
        {
            Position newPosition;
            float angle = this->_angularSpeed * totalTime;
            // Avance vers la gauche tout en faisant des cercles
            newPosition.x = this->_startX - (this->_xSpeed * totalTime) + this->_radius * std::cos(angle);
            newPosition.y = this->_centerY + this->_radius * std::sin(angle);
            return newPosition;
        }

    private:
        float _xSpeed;
        float _radius;
        float _angularSpeed;
        float _startX;
        float _centerY;
};

#endif // CIRCULARSTRATEGY_HPP_
