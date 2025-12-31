#ifndef BOIDHANDLER_H
#define BOIDHANDLER_H

#include <vector>
#include "Vec2D.h"

// Forward declarations
struct Boid;

class boidHandler {
public:
    virtual ~boidHandler() = default;
    virtual std::vector<Boid*> getBoidsInRange(const Vec2& position, float range) = 0;
    virtual void setBoids(const std::vector<Boid*>& allBoids) = 0;
};

#endif