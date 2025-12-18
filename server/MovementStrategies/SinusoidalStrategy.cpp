/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Sinusoidal Movement Strategy Implementation
*/

#include "SinusoidalStrategy.hpp"
#include <cmath>

SinusoidalStrategy::SinusoidalStrategy(float xSpeed, float amplitude,
                                       float frequency, float centerLineY,
                                       float startX)
    : _xSpeed(xSpeed), _amplitude(amplitude), _frequency(frequency),
      _centerLineY(centerLineY), _startX(startX) {}

Position
SinusoidalStrategy::update([[maybe_unused]] const Position &currentPosition,
                           float totalTime) {
  Position newPosition;
  newPosition.x = this->_startX - (this->_xSpeed * totalTime);
  newPosition.y = this->_centerLineY +
                  this->_amplitude * std::sin(this->_frequency * totalTime);
  return newPosition;
}
