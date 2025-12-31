#include "raylib.h"
#include <vector>
#include <cmath>
#include "Boid.h"
#include "Sidebar.h"

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

        stepBoids(boids, sidebar, dt, W, H);

        // Update FPS graph and sliders
        sidebar.update(GetFPS());

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
