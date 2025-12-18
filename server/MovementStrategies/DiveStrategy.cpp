/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Dive Attack Movement Strategy Implementation
*/

#include "DiveStrategy.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DiveStrategy::DiveStrategy(float xSpeed, float diveDepth, float diveDuration, float startX, float startY)
    : _xSpeed(xSpeed), _diveDepth(diveDepth), _diveDuration(diveDuration), _startX(startX), _startY(startY)
{
}

Position DiveStrategy::update([[maybe_unused]] const Position &currentPosition, float totalTime)
{
    Position newPosition;
    newPosition.x = this->_startX - (this->_xSpeed * totalTime);
    float cycleTime = std::fmod(totalTime, this->_diveDuration);
    float normalizedTime = cycleTime / this->_diveDuration;
    float diveOffset = std::sin(M_PI * normalizedTime) * this->_diveDepth;
    newPosition.y = this->_startY + diveOffset;
    return newPosition;
}
