/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Sinusoidal Movement Strategy
*/

#ifndef SINUSOIDALSTRATEGY_HPP_
#define SINUSOIDALSTRATEGY_HPP_

#include "IMovementStrategy.hpp"
#include <cmath>

class SinusoidalStrategy : public IMovementStrategy {
    public:
        SinusoidalStrategy(float xSpeed, float amplitude, float frequency, float centerLineY, float startX)
            : _xSpeed(xSpeed), _amplitude(amplitude), _frequency(frequency), _centerLineY(centerLineY), _startX(startX) {}

        Position update([[maybe_unused]] const Position &currentPosition, float totalTime) override
        {
            Position newPosition;
            newPosition.x = this->_startX - (this->_xSpeed * totalTime);
            newPosition.y = this->_centerLineY + this->_amplitude * std::sin(this->_frequency * totalTime);
            return newPosition;
        }

    private:
        float _xSpeed;
        float _amplitude;
        float _frequency;
        float _centerLineY;
        float _startX;
};

#endif // SINUSOIDALSTRATEGY_HPP_
