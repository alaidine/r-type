/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Dive Attack Movement Strategy
*/

#ifndef DIVESTRATEGY_HPP_
#define DIVESTRATEGY_HPP_

#include "IMovementStrategy.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class DiveStrategy : public IMovementStrategy {
    public:
        DiveStrategy(float xSpeed, float diveDepth, float diveDuration, float startX, float startY)
            : _xSpeed(xSpeed), _diveDepth(diveDepth), _diveDuration(diveDuration), _startX(startX), _startY(startY) {}

        Position update([[maybe_unused]] const Position &currentPosition, float totalTime) override
        {
            Position newPosition;
            newPosition.x = this->_startX - (this->_xSpeed * totalTime);
            float cycleTime = std::fmod(totalTime, this->_diveDuration);
            float normalizedTime = cycleTime / this->_diveDuration;
            float diveOffset = std::sin(M_PI * normalizedTime) * this->_diveDepth;
            newPosition.y = this->_startY + diveOffset;
            return newPosition;
        }

    private:
        float _xSpeed;
        float _diveDepth;
        float _diveDuration;
        float _startX;
        float _startY;
};

#endif // DIVESTRATEGY_HPP_
