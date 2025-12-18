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
        LinearStrategy(float xSpeed, float ySpeed, float startX, float startY);
        Position update(const Position &currentPosition, float totalTime) override;

    private:
        float _xSpeed;
        float _ySpeed;
        float _startX;
        float _startY;
};

#endif // LINEARSTRATEGY_HPP_
