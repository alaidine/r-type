/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ZigZag Movement Strategy
*/

#ifndef ZIGZAGSTRATEGY_HPP_
#define ZIGZAGSTRATEGY_HPP_

#include "IMovementStrategy.hpp"

class ZigZagStrategy : public IMovementStrategy {
public:
  ZigZagStrategy(float xSpeed, float amplitude, float frequency, float startX,
                 float startY);
  Position update(const Position &currentPosition, float totalTime) override;

private:
  float _xSpeed;
  float _amplitude;
  float _frequency;
  float _startX;
  float _startY;
};

#endif // ZIGZAGSTRATEGY_HPP_
