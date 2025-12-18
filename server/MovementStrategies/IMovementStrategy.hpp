/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Movement Strategy Interface for Server-side Mob Patterns
*/

#ifndef IMOVEMENTSTRATEGY_HPP_
#define IMOVEMENTSTRATEGY_HPP_

struct Position {
    float x;
    float y;
};

class IMovementStrategy {
    public:
        virtual ~IMovementStrategy() = default;
        virtual Position update(const Position &currentPosition, float totalTime) = 0;
};

#endif // IMOVEMENTSTRATEGY_HPP_
