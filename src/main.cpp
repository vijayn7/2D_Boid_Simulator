#include "raylib.h"
#include <vector>
#include <cmath>
#include "Boid.h"

static void wrap(Vec2& p, int W, int H) {
    if (p.x < 0) p.x += W;
    if (p.x >= W) p.x -= W;
    if (p.y < 0) p.y += H;
    if (p.y >= H) p.y -= H;
}

static void drawBoid(const Boid& b) {
    Vec2 v = b.vel;
    if (v.lengthSquared() < 1e-6f) v = {1, 0};

    float ang = std::atan2(v.y, v.x);

    float size = 8.0f;
    Vec2 forward = { std::cos(ang), std::sin(ang) };
    Vec2 right   = { std::cos(ang + 3.1415926f/2.0f), std::sin(ang + 3.1415926f/2.0f) };

    Vec2 tip   = b.pos + forward * (size * 1.6f);
    Vec2 left  = b.pos - forward * (size * 1.0f) - right * (size * 0.9f);
    Vec2 rightP= b.pos - forward * (size * 1.0f) + right * (size * 0.9f);

    DrawTriangle(
        {tip.x, tip.y},
        {left.x, left.y},
        {rightP.x, rightP.y},
        BLACK
    );
}

int main() {
    const int W = 1000;
    const int H = 700;

    InitWindow(W, H, "Boids - raylib");
    SetTargetFPS(60);

    std::vector<Boid> boids;
    const int N = 200;

    for (int i = 0; i < N; i++) {
        Boid b;
        b.pos = { (float)GetRandomValue(0, W-1), (float)GetRandomValue(0, H-1) };
        b.vel = { (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
        boids.push_back(b);
    }

    const float maxSpeed = 140.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (auto& b : boids) {
            b.update(dt, maxSpeed);
            wrap(b.pos, W, H);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (const auto& b : boids) drawBoid(b);

        DrawText("Step 2: moving boids (no flocking yet)", 20, 20, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
