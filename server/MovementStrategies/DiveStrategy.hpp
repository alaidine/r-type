/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Dive Attack Movement Strategy
*/

#ifndef DIVESTRATEGY_HPP_
#define DIVESTRATEGY_HPP_

#include "IMovementStrategy.hpp"

class DiveStrategy : public IMovementStrategy {
    public:
        DiveStrategy(float xSpeed, float diveDepth, float diveDuration, float startX, float startY);
        Position update(const Position &currentPosition, float totalTime) override;

    private:
        float _xSpeed;
        float _diveDepth;
        float _diveDuration;
        float _startX;
        float _startY;
};

#endif // DIVESTRATEGY_HPP_
