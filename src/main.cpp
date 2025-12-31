#include "raylib.h"
#include <vector>
#include <cmath>
#include <memory>
#include "Boid.h"
#include "Sidebar.h"
#include "boidHandler.h"
#include "bruteForceHandler.cpp"
#include "gridHandler.cpp"

// Function to update boids using the handler interface
static void stepBoids(std::vector<Boid>& boids, const Sidebar& sidebar, boidHandler* handler, float dt, int W, int H) {
    
    // Create vector of pointers for handler
    std::vector<Boid*> boidPtrs;
    boidPtrs.reserve(boids.size());
    for (auto& b : boids) {
        boidPtrs.push_back(&b);
    }
    
    // Update handler with current boid positions
    handler->setBoids(boidPtrs);
    
    for (auto& b : boids) {
        // Get nearby boids using the handler
        float maxRadius = std::max(sidebar.params.neighborRadius, sidebar.params.separationRadius);
        std::vector<Boid*> nearbyBoids = handler->getBoidsInRange(b.pos, maxRadius);
        
        // Compute forces based on nearby boids
        Vec2 sep = b.separation(nearbyBoids, sidebar.params.separationRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);
        Vec2 ali = b.alignment(nearbyBoids, sidebar.params.neighborRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);
        Vec2 coh = b.cohesion(nearbyBoids, sidebar.params.neighborRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);

        b.applyForce(sep * sidebar.params.separationWeight);
        b.applyForce(ali * sidebar.params.alignmentWeight);
        b.applyForce(coh * sidebar.params.cohesionWeight);
    }

    for (auto& b : boids) {
        b.update(dt, sidebar.params.maxSpeed);
        wrap(b.pos, W, H);
    }

    // Handle boid count changes
    int targetBoids = sidebar.params.numBoids;
    if (targetBoids > (int)boids.size()) {
        // Spawn new boids
        for (size_t i = boids.size(); i < (size_t)targetBoids; i++) {
            Boid b;
            b.pos = { (float)GetRandomValue(0, W-1), (float)GetRandomValue(0, H-1) };
            b.vel = { (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
            boids.push_back(b);
        }
    } else if (targetBoids < (int)boids.size()) {
        // Remove excess boids
        boids.resize(targetBoids);
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

    // Create handler implementations
    std::unique_ptr<boidHandler> bruteForce = std::make_unique<bruteForceHandler>();
    std::unique_ptr<boidHandler> gridSpatial = std::make_unique<gridHandler>(W, H, 50);
    boidHandler* currentHandler = bruteForce.get();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Select current handler based on UI
        currentHandler = sidebar.params.useGridHandler ? gridSpatial.get() : bruteForce.get();

        stepBoids(boids, sidebar, currentHandler, dt, W, H);

        // Update FPS graph and sliders
        sidebar.update(GetFPS());

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw grid lines if enabled and using grid handler (draw first so boids appear on top)
        if (sidebar.params.showGridLines && sidebar.params.useGridHandler) {
            gridHandler* gh = static_cast<gridHandler*>(gridSpatial.get());
            gh->drawGrid();
        }

        // Draw boids on the left
        for (const auto& b : boids) drawBoid(b, sidebar);

        // Draw border line between boid section and FPS graph
        DrawLine(W, 0, W, H, RAYWHITE);

        // Draw FPS graph on the right
        sidebar.draw(W + 20, 20, graphW, graphH);

        EndDrawing();

    }

    CloseWindow();
    return 0;
}
