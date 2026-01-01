#ifndef BRUTEFORCEHANDLER_H
#define BRUTEFORCEHANDLER_H

#include <vector>
#include "Boid.h"
#include "boidHandler.h"

class bruteForceHandler : public boidHandler {
    std::vector<Boid*> boids;

public:

    std::vector<Boid*> getBoidsInRange(const Vec2& position, float range) override {
        std::vector<Boid*> nearbyBoids;
        float r2 = range * range;

        for (auto& b : boids) {
            if (distSquared(position, b->pos) <= r2) {
                nearbyBoids.push_back(b);
            }
        }

        return nearbyBoids;
    }

    void setBoids(const std::vector<Boid*>& allBoids) override {
        boids = allBoids;
    }

};

#endif