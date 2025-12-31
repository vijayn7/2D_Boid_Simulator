#pragma once
#include "Vec2D.h"

struct Boid {
    Vec2 pos;
    Vec2 vel;
    Vec2 acc;

    void update(float dt, float maxSpeed) {
        vel += acc * dt;
        vel = clampMag(vel, maxSpeed);
        pos += vel * dt;
        acc = {0,0};
    }
};