/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Circular Movement Strategy Implementation
*/

#include "CircularStrategy.hpp"
#include <cmath>

CircularStrategy::CircularStrategy(float xSpeed, float radius,
                                   float angularSpeed, float startX,
                                   float centerY)
    : _xSpeed(xSpeed), _radius(radius), _angularSpeed(angularSpeed),
      _startX(startX), _centerY(centerY) {}

Position
CircularStrategy::update([[maybe_unused]] const Position &currentPosition,
                         float totalTime) {
  Position newPosition;
  float angle = this->_angularSpeed * totalTime;
  newPosition.x = this->_startX - (this->_xSpeed * totalTime) +
                  this->_radius * std::cos(angle);
  newPosition.y = this->_centerY + this->_radius * std::sin(angle);
  return newPosition;
}
