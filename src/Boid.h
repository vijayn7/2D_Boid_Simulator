#pragma once
#include "Vec2D.h"
#include <vector>

struct Boid {
    Vec2 pos;
    Vec2 vel;
    Vec2 acc;

    void applyForce(const Vec2& f) { acc += f; }

    void update(float dt, float maxSpeed) {
        vel += acc * dt;
        vel = clampMag(vel, maxSpeed);
        pos += vel * dt;
        acc = {0,0};
    }

    Vec2 cohesion(const std::vector<Boid>& boids, float neighborRadius, float maxSpeed, float maxForce) const {
        float r2 = neighborRadius * neighborRadius;

        Vec2 center = {0,0};
        int count = 0;

        for (const auto& other : boids) {
            if (&other == this) continue;
            if (dist2(pos, other.pos) < r2) {
                center += other.pos;
                count++;
            }
        }

        if (count == 0) return {0,0};

        center = center / (float)count;

        Vec2 desired = center - pos;
        desired = normalize(desired) * maxSpeed;

        Vec2 steer = desired - vel;
        return limitForce(steer, maxForce);
    }

    Vec2 alignment(const std::vector<Boid>& boids, float neighborRadius, float maxSpeed, float maxForce) const {
        float r2 = neighborRadius * neighborRadius;

        Vec2 avgVel = {0,0};
        int count = 0;

        for (const auto& other : boids) {
            if (&other == this) continue;
            if (dist2(pos, other.pos) < r2) {
                avgVel += other.vel;
                count++;
            }
        }

        if (count == 0) return {0,0};

        avgVel = avgVel / (float)count;

        Vec2 desired = normalize(avgVel) * maxSpeed;

        Vec2 steer = desired - vel;
        return limitForce(steer, maxForce);
    }

    Vec2 separation(const std::vector<Boid>& boids, float separationRadius, float maxSpeed, float maxForce) const {
        float r2 = separationRadius * separationRadius;

        Vec2 push = {0,0};
        int count = 0;

        for (const auto& other : boids) {
            if (&other == this) continue;

            float d2 = dist2(pos, other.pos);
            if (d2 > 0.0001f && d2 < r2) {
                Vec2 away = pos - other.pos;
                away = away / d2; // stronger when closer
                push += away;
                count++;
            }
        }

        if (count == 0) return {0,0};

        push = push / (float)count;
        Vec2 desired = normalize(push) * maxSpeed;

        Vec2 steer = desired - vel;
        return limitForce(steer, maxForce);
    }
};
