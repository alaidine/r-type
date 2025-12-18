/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Sinusoidal Movement Strategy
*/

#ifndef SINUSOIDALSTRATEGY_HPP_
#define SINUSOIDALSTRATEGY_HPP_

#include "IMovementStrategy.hpp"

class SinusoidalStrategy : public IMovementStrategy {
    public:
        SinusoidalStrategy(float xSpeed, float amplitude, float frequency, float centerLineY, float startX);
        Position update(const Position &currentPosition, float totalTime) override;

    private:
        float _xSpeed;
        float _amplitude;
        float _frequency;
        float _centerLineY;
        float _startX;
};

#endif // SINUSOIDALSTRATEGY_HPP_
