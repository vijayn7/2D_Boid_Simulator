#pragma once
#include "Vec2D.h"
#include <vector>
#include <cmath>
#include "Sidebar.h"

// Forward declaration
class boidHandler;

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

    // Flocking behaviors: This make the boid steer towards the average position of local flockmates
    Vec2 cohesion(const std::vector<Boid*>& boids, float neighborRadius, float maxSpeed, float maxForce) const {
        // sensing distance squared
        float r2 = neighborRadius * neighborRadius;

        Vec2 center = {0,0};
        int count = 0;

        for (const auto* other : boids) {
            if (other == this) continue;
            if (distSquared(pos, other->pos) <= r2) {
                center += other->pos;
                count++;
            }
        }

        if (count == 0) return {0,0};

        center = center / (float)count;

        Vec2 desired = center - pos;
        desired = normalize(desired) * maxSpeed;

        // Steering = Desired position - Current velocity
        Vec2 steer = desired - vel;
        return limitForce(steer, maxForce);
    }

    // This makes the boid align its velocity with the average velocity of local flockmates
    Vec2 alignment(const std::vector<Boid*>& boids, float neighborRadius, float maxSpeed, float maxForce) const {
        float r2 = neighborRadius * neighborRadius;

        Vec2 avgVel = {0,0};
        int count = 0;

        for (const auto* other : boids) {
            if (other == this) continue;
            if (distSquared(pos, other->pos) <= r2) {
                avgVel += other->vel;
                count++;
            }
        }

        if (count == 0) return {0,0};

        avgVel = avgVel / (float)count;

        Vec2 desired = normalize(avgVel) * maxSpeed;

        Vec2 steer = desired - vel;
        return limitForce(steer, maxForce);
    }

    // This makes the boid steer to avoid crowding local flockmates
    Vec2 separation(const std::vector<Boid*>& boids, float separationRadius, float maxSpeed, float maxForce) const {
        float r2 = separationRadius * separationRadius;

        Vec2 push = {0,0};
        int count = 0;

        for (const auto* other : boids) {
            if (other == this) continue;

            float d2 = distSquared(pos, other->pos);
            if (d2 > 0.0001f && d2 < r2) {
                Vec2 away = pos - other->pos;
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

static void drawVelocityVector(const Boid& b, float birdSize) {
    Vec2 velEnd = b.pos + normalize(b.vel) * birdSize * 4.0f;
    DrawLineEx({b.pos.x, b.pos.y}, {velEnd.x, velEnd.y}, 1.0f, BLUE);
}

static void DrawAdditionalDebugInfo(const Boid& b, const Sidebar& sidebar) {
    // Draw velocity vector
    if (sidebar.getParams().drawVelocityVectors) drawVelocityVector(b, sidebar.getParams().birdSize);
    
}

static void drawBoid(const Boid& b, const Sidebar& sidebar) {

    float pi = 3.1415926f;
    float halfPi = pi / 2.0f;

    Vec2 v = b.vel;
    if (v.lengthSquared() < 1e-6f) v = {1, 0};

    float ang = std::atan2(v.y, v.x);

    float size = sidebar.getParams().birdSize;

    // Calculate the triangle points from the center position and orientation
    Vec2 forward = { std::cos(ang), std::sin(ang) }; // Tip of the boid
    Vec2 right   = { std::cos(ang + halfPi), std::sin(ang + halfPi) }; // Right side

    float pointScale = 1.6f;
    float sideSpan   = 1.0f;
    float sideOffset = 0.9f;

    Vec2 tip   = b.pos + forward * (size * pointScale);
    Vec2 left  = b.pos - forward * (size * sideSpan) - right * (size * sideOffset);
    Vec2 rightP= b.pos - forward * (size * sideSpan) + right * (size * sideOffset);

    DrawTriangle(
        {tip.x, tip.y},
        {left.x, left.y},
        {rightP.x, rightP.y},
        RAYWHITE
    );

    DrawAdditionalDebugInfo(b, sidebar);

}

// Keep boids within window bounds and bounce off edges
static void constrainToWindow(Boid& b, int W, int H) {
    const float margin = 5.0f;  // Bounce slightly before hitting edge
    
    // Bounce off left/right walls
    if (b.pos.x < margin) {
        b.pos.x = margin;
        b.vel.x = std::abs(b.vel.x);  // Reverse and ensure positive direction
    } else if (b.pos.x > W - margin) {
        b.pos.x = W - margin;
        b.vel.x = -std::abs(b.vel.x);  // Reverse and ensure negative direction
    }
    
    // Bounce off top/bottom walls
    if (b.pos.y < margin) {
        b.pos.y = margin;
        b.vel.y = std::abs(b.vel.y);  // Reverse and ensure positive direction
    } else if (b.pos.y > H - margin) {
        b.pos.y = H - margin;
        b.vel.y = -std::abs(b.vel.y);  // Reverse and ensure negative direction
    }
};
