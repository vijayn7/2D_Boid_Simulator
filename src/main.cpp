#include "raylib.h"
#include <vector>
#include <cmath>
#include "Boid.h"
#include "Sidebar.h"

// Wrap around the screen edges
static void wrap(Vec2& p, int W, int H) {
    if (p.x < 0) p.x += W;
    if (p.x >= W) p.x -= W;
    if (p.y < 0) p.y += H;
    if (p.y >= H) p.y -= H;
}

static void drawBoid(const Boid& b, float birdSize, bool debugMode) {

    float pi = 3.1415926f;
    float halfPi = pi / 2.0f;

    Vec2 v = b.vel;
    if (v.lengthSquared() < 1e-6f) v = {1, 0};

    float ang = std::atan2(v.y, v.x);

    float size = birdSize;

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

    // Debug: draw velocity vector, center point
    if (debugMode) {
        DrawCircleV({b.pos.x, b.pos.y}, 2.0f, RED);
        DrawLineV({b.pos.x, b.pos.y}, {b.pos.x + v.x * 0.2f, b.pos.y + v.y * 0.2f}, BLUE);
    }

}

int main() {
    const int W = 1000;
    const int H = 700;
    const int graphW = 350;
    const int graphH = 300;

    InitWindow(W + graphW + 40, H, "Boids - raylib");
    SetTargetFPS(240);

    std::vector<Boid> boids;
    const int N = 500;

    // Initialize boids with random positions and velocities
    for (int i = 0; i < N; i++) {
        Boid b;
        b.pos = { (float)GetRandomValue(0, W-1), (float)GetRandomValue(0, H-1) };
        b.vel = { (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
        boids.push_back(b);
    }

    const float maxSpeed = 140.0f;

    const float neighborRadius = 70.0f;
    const float maxForce = 220.0f;
    const float separationRadius = 28.0f;

    const float alignmentWeight = 1.0f;
    const float cohesionWeight  = 0.8f;
    const float separationWeight = 1.6f;

    Sidebar sidebar;
    // Initialize sidebar params with default values
    sidebar.params.maxSpeed = maxSpeed;
    sidebar.params.neighborRadius = neighborRadius;
    sidebar.params.maxForce = maxForce;
    sidebar.params.separationRadius = separationRadius;
    sidebar.params.alignmentWeight = alignmentWeight;
    sidebar.params.cohesionWeight = cohesionWeight;
    sidebar.params.separationWeight = separationWeight;
    sidebar.params.numBoids = 500.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (auto& b : boids) {
            Vec2 sep = b.separation(boids, sidebar.params.separationRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);
            Vec2 ali = b.alignment(boids,  sidebar.params.neighborRadius,  sidebar.params.maxSpeed, sidebar.params.maxForce);
            Vec2 coh = b.cohesion(boids,   sidebar.params.neighborRadius,  sidebar.params.maxSpeed, sidebar.params.maxForce);

            b.applyForce(sep * sidebar.params.separationWeight);
            b.applyForce(ali * sidebar.params.alignmentWeight);
            b.applyForce(coh * sidebar.params.cohesionWeight);
        }

        for (auto& b : boids) {
            b.update(dt, sidebar.params.maxSpeed);
            wrap(b.pos, W, H);
        }

        // Update FPS graph and sliders
        sidebar.update(GetFPS());
        
        // Handle boid count changes
        int targetBoids = (int)sidebar.params.numBoids;
        if (targetBoids > boids.size()) {
            // Spawn new boids
            for (int i = boids.size(); i < targetBoids; i++) {
                Boid b;
                b.pos = { (float)GetRandomValue(0, W-1), (float)GetRandomValue(0, H-1) };
                b.vel = { (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
                boids.push_back(b);
            }
        } else if (targetBoids < boids.size()) {
            // Remove excess boids
            boids.resize(targetBoids);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw boids on the left
        for (const auto& b : boids) drawBoid(b, sidebar.params.birdSize, sidebar.params.debugMode);

        // Draw debug info if enabled
        if (sidebar.params.debugMode) {
            DrawText(TextFormat("Boids: %d", (int)boids.size()), 10, 10, 12, RAYWHITE);
        }

        // Draw border line between boid section and FPS graph
        DrawLine(W, 0, W, H, RAYWHITE);

        // Draw FPS graph on the right
        sidebar.draw(W + 20, 20, graphW, graphH);

        EndDrawing();

    }

    CloseWindow();
    return 0;
}
