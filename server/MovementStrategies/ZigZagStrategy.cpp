/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ZigZag Movement Strategy Implementation
*/

#include "ZigZagStrategy.hpp"
#include <cmath>

ZigZagStrategy::ZigZagStrategy(float xSpeed, float amplitude, float frequency, float startX, float startY)
    : _xSpeed(xSpeed), _amplitude(amplitude), _frequency(frequency), _startX(startX), _startY(startY)
{
}

Position ZigZagStrategy::update([[maybe_unused]] const Position &currentPosition, float totalTime)
{
    Position newPosition;
    newPosition.x = this->_startX - (this->_xSpeed * totalTime);
    float phase = std::fmod(this->_frequency * totalTime, 2.0f);
    float zigzag = (phase < 1.0f) ? phase : (2.0f - phase);
    newPosition.y = this->_startY + this->_amplitude * (zigzag * 2.0f - 1.0f);
    return newPosition;
}
