/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Circular Movement Strategy - Avance en faisant des cercles
*/

#ifndef CIRCULARSTRATEGY_HPP_
#define CIRCULARSTRATEGY_HPP_

#include "IMovementStrategy.hpp"

class CircularStrategy : public IMovementStrategy {
    public:
        CircularStrategy(float xSpeed, float radius, float angularSpeed, float startX, float centerY);
        Position update(const Position &currentPosition, float totalTime) override;

    private:
        float _xSpeed;
        float _radius;
        float _angularSpeed;
        float _startX;
        float _centerY;
};

#endif // CIRCULARSTRATEGY_HPP_
