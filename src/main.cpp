#include "raylib.h"
#include <vector>
#include <cmath>
#include <memory>
#include "Boid.h"
#include "Sidebar.h"
#include "boidHandler.h"
#include "bruteForceHandler.h"
#include "gridHandler.h"

int main() {
    const int W = 1500;
    const int H = 1000;
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
    sidebar.params.numBoids = 500;

    // Create handler implementations
    std::unique_ptr<boidHandler> bruteForce = std::make_unique<bruteForceHandler>();
    std::unique_ptr<boidHandler> gridSpatial = std::make_unique<gridHandler>(W, H, sidebar.params.maxBoidsPerCell);
    boidHandler* currentHandler = bruteForce.get();
    int lastMaxBoidsPerCell = sidebar.params.maxBoidsPerCell;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Recreate gridHandler if maxBoidsPerCell changed
        if (lastMaxBoidsPerCell != sidebar.params.maxBoidsPerCell) {
            gridSpatial = std::make_unique<gridHandler>(W, H, sidebar.params.maxBoidsPerCell);
            lastMaxBoidsPerCell = sidebar.params.maxBoidsPerCell;
        }

        // Select current handler based on UI
        currentHandler = sidebar.params.useGridHandler ? gridSpatial.get() : bruteForce.get();

        // ====== STEP BOIDS ======
        // Create vector of pointers for handler
        std::vector<Boid*> boidPtrs;
        boidPtrs.reserve(boids.size());
        for (auto& b : boids) {
            boidPtrs.push_back(&b);
        }
        
        // Update handler with current boid positions
        currentHandler->setBoids(boidPtrs);
        
        // Calculate forces
        for (auto& b : boids) {
            float maxRadius = std::max(sidebar.params.neighborRadius, sidebar.params.separationRadius);
            std::vector<Boid*> nearbyBoids = currentHandler->getBoidsInRange(b.pos, maxRadius);
            
            Vec2 sep = b.separation(nearbyBoids, sidebar.params.separationRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);
            Vec2 ali = b.alignment(nearbyBoids, sidebar.params.neighborRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);
            Vec2 coh = b.cohesion(nearbyBoids, sidebar.params.neighborRadius, sidebar.params.maxSpeed, sidebar.params.maxForce);

            b.applyForce(sep * sidebar.params.separationWeight);
            b.applyForce(ali * sidebar.params.alignmentWeight);
            b.applyForce(coh * sidebar.params.cohesionWeight);
            
            // Apply wall avoidance force
            const float wallDistance = 150.0f;  // Distance at which to start avoiding walls
            Vec2 wallAvoid = {0, 0};
            
            // Left wall
            if (b.pos.x < wallDistance) {
                float force = (wallDistance - b.pos.x) / wallDistance;
                wallAvoid.x += force;
            }
            // Right wall
            if (b.pos.x > W - wallDistance) {
                float force = (b.pos.x - (W - wallDistance)) / wallDistance;
                wallAvoid.x -= force;
            }
            // Top wall
            if (b.pos.y < wallDistance) {
                float force = (wallDistance - b.pos.y) / wallDistance;
                wallAvoid.y += force;
            }
            // Bottom wall
            if (b.pos.y > H - wallDistance) {
                float force = (b.pos.y - (H - wallDistance)) / wallDistance;
                wallAvoid.y -= force;
            }
            
            // Apply wall avoidance as a force with weight control
            if (wallAvoid.lengthSquared() > 0) {
                wallAvoid = normalize(wallAvoid) * sidebar.params.maxForce * sidebar.params.wallAvoidanceWeight;
                b.applyForce(wallAvoid);
            }
            
            // Apply mouse avoidance force
            Vector2 mousePos = GetMousePosition();
            Vec2 mouse = {mousePos.x, mousePos.y};
            Vec2 mouseDir = b.pos - mouse;
            float mouseDist = std::sqrt(mouseDir.lengthSquared());
            
            const float mouseAvoidanceRadius = 200.0f;  // Distance at which to start avoiding mouse
            if (mouseDist < mouseAvoidanceRadius && mouseDist > 0.01f) {
                float force = (mouseAvoidanceRadius - mouseDist) / mouseAvoidanceRadius;
                Vec2 mouseAvoid = normalize(mouseDir) * sidebar.params.maxForce * sidebar.params.mouseAvoidanceWeight * force;
                b.applyForce(mouseAvoid);
            }
        }

        // Update positions and clamp to window bounds
        for (auto& b : boids) {
            b.update(dt, sidebar.params.maxSpeed);
            
            // Clamp boid position to stay within bounds
            if (b.pos.x < 0) b.pos.x = 0;
            if (b.pos.x > W) b.pos.x = W;
            if (b.pos.y < 0) b.pos.y = 0;
            if (b.pos.y > H) b.pos.y = H;
        }

        // Handle boid count changes
        int targetBoids = sidebar.params.numBoids;
        if (targetBoids > (int)boids.size()) {
            for (size_t i = boids.size(); i < (size_t)targetBoids; i++) {
                Boid b;
                b.pos = { (float)GetRandomValue(0, W-1), (float)GetRandomValue(0, H-1) };
                b.vel = { (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
                boids.push_back(b);
            }
        } else if (targetBoids < (int)boids.size()) {
            boids.resize(targetBoids);
        }

        // Update FPS graph and sliders
        sidebar.update(GetFPS());

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw boids on the left
        for (const auto& b : boids) drawBoid(b, sidebar);

        // Draw grid lines/heatmap on top if enabled and using grid handler
        if ((sidebar.params.showGridLines || sidebar.params.showHeatmap) && sidebar.params.useGridHandler) {
            gridHandler* gh = static_cast<gridHandler*>(gridSpatial.get());
            gh->drawGrid(sidebar.params.showHeatmap);
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
