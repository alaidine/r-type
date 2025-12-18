/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Linear Movement Strategy Implementation
*/

#include "LinearStrategy.hpp"

LinearStrategy::LinearStrategy(float xSpeed, float ySpeed, float startX,
                               float startY)
    : _xSpeed(xSpeed), _ySpeed(ySpeed), _startX(startX), _startY(startY) {}

Position
LinearStrategy::update([[maybe_unused]] const Position &currentPosition,
                       float totalTime) {
  Position newPosition;
  newPosition.x = this->_startX - (this->_xSpeed * totalTime);
  newPosition.y = this->_startY + (this->_ySpeed * totalTime);
  return newPosition;
}
