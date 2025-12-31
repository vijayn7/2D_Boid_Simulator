#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "raylib.h"
#include <deque>
#include <algorithm>
#include <vector>

// Structure to hold boid parameters
struct BoidParams {
    float maxSpeed = 140.0f;
    float neighborRadius = 70.0f;
    float maxForce = 220.0f;
    float separationRadius = 28.0f;
    float alignmentWeight = 1.0f;
    float cohesionWeight = 0.8f;
    float separationWeight = 1.6f;
    float birdSize = 8.0f;
    float numBoids = 500.0f;

    bool drawVelocityVectors = false;
};

// Slider struct for UI
struct Slider {
    const char* label;
    float* value;
    float minVal;
    float maxVal;
    float x, y;
    float width;
    float height;

    void draw() {
        // Label with fixed width for alignment
        DrawText(label, x, y - 12, 11, RAYWHITE);
        
        // Slider background with rounded effect
        DrawRectangle(x, y + 2, width, height, {40, 40, 40, 200});
        DrawRectangleLines(x, y + 2, width, height, {80, 80, 80, 200});
        
        // Slider value position
        float normalizedValue = (*value - minVal) / (maxVal - minVal);
        float sliderX = x + normalizedValue * width;
        
        // Slider thumb - styled
        DrawRectangle(sliderX - 5, y - 2, 10, height + 8, RAYWHITE);
        DrawRectangleLines(sliderX - 5, y - 2, 10, height + 8, {150, 150, 150, 255});
        
        // Value display on the right
        DrawText(TextFormat("%.1f", *value), x + width + 15, y - 6, 11, RAYWHITE);
    }

    void update() {
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) return;
        
        Vector2 mousePos = GetMousePosition();
        
        // Check if mouse is over slider (with some tolerance)
        if (mousePos.x >= x && mousePos.x <= x + width &&
            mousePos.y >= y - 5 && mousePos.y <= y + 15) {
            
            // Calculate new value based on mouse position
            float normalizedPos = (mousePos.x - x) / width;
            normalizedPos = std::max(0.0f, std::min(1.0f, normalizedPos));
            *value = minVal + normalizedPos * (maxVal - minVal);
        }
    }
};

// Checkbox struct for UI
struct Checkbox {
    const char* label;
    bool* value;
    float x, y;
    int size;

    void draw() {
        // Checkbox background (before label)
        int checkboxX = x;
        int checkboxY = y - 4;
        DrawRectangle(checkboxX, checkboxY, size, size, {40, 40, 40, 200});
        DrawRectangleLines(checkboxX, checkboxY, size, size, {80, 80, 80, 200});
        
        // Checkmark if enabled
        if (*value) {
            DrawLine(checkboxX + 2, checkboxY + 7, checkboxX + 6, checkboxY + 11, RAYWHITE);
            DrawLine(checkboxX + 6, checkboxY + 11, checkboxX + 12, checkboxY + 3, RAYWHITE);
        }
        
        // Label (after checkbox)
        DrawText(label, x + 25, y - 2, 12, RAYWHITE);
    }

    void update() {
        if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
        
        Vector2 mousePos = GetMousePosition();
        int checkboxX = x;
        int checkboxY = y - 4;
        
        // Check if clicked on checkbox
        if (mousePos.x >= checkboxX && mousePos.x <= checkboxX + size &&
            mousePos.y >= checkboxY && mousePos.y <= checkboxY + size) {
            *value = !*value;
        }
    }
};

// FPS Graph tracker for sidebar display
class Sidebar {
public:
    static const int maxSamples = 1080;
    std::deque<float> fpsHistory;
    BoidParams params;

    Sidebar() {
        // Initialize sliders
        float sliderWidth = 150;
        float sliderHeight = 8;
        float sliderSpacing = 30;

        sliders.push_back({"Max Speed", &params.maxSpeed, 50.0f, 300.0f, 0, 0, sliderWidth, sliderHeight});
        sliders.push_back({"Neighbor Rad", &params.neighborRadius, 20.0f, 150.0f, 0, sliderSpacing, sliderWidth, sliderHeight});
        sliders.push_back({"Max Force", &params.maxForce, 50.0f, 400.0f, 0, sliderSpacing * 2, sliderWidth, sliderHeight});
        sliders.push_back({"Sep Radius", &params.separationRadius, 5.0f, 80.0f, 0, sliderSpacing * 3, sliderWidth, sliderHeight});
        sliders.push_back({"Align Weight", &params.alignmentWeight, 0.0f, 3.0f, 0, sliderSpacing * 4, sliderWidth, sliderHeight});
        sliders.push_back({"Cohesion W", &params.cohesionWeight, 0.0f, 3.0f, 0, sliderSpacing * 5, sliderWidth, sliderHeight});
        sliders.push_back({"Sep Weight", &params.separationWeight, 0.0f, 3.0f, 0, sliderSpacing * 6, sliderWidth, sliderHeight});
        sliders.push_back({"Bird Size", &params.birdSize, 2.0f, 20.0f, 0, sliderSpacing * 7, sliderWidth, sliderHeight});
        sliders.push_back({"Num Boids", &params.numBoids, 10.0f, 1000.0f, 0, sliderSpacing * 8, sliderWidth, sliderHeight});
        
        // Initialize checkboxes
        checkboxes.push_back({"Draw Velocity Vectors", &params.drawVelocityVectors, 0, 0, 14});
    }

    void update(float fps) {
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > maxSamples) {
            fpsHistory.pop_front();
        }

        // Update sliders
        for (auto& slider : sliders) {
            slider.update();
        }
        
        // Update checkboxes
        for (auto& checkbox : checkboxes) {
            checkbox.update();
        }
    }

    void draw(int x, int y, int width, int height) {
        // Background - more transparent
        DrawRectangle(x, y, width, 160, {20, 20, 20, 120});
        DrawRectangleLines(x, y, width, 160, {100, 100, 100, 200});

        // Title
        DrawText("FPS Monitor", x + 10, y + 8, 16, RAYWHITE);
        DrawLine(x + 10, y + 25, x + width - 10, y + 25, {100, 100, 100, 100});
        
        // Current FPS display
        float currentFps = fpsHistory.empty() ? 0 : fpsHistory.back();
        Color fpsColor = (currentFps > 55) ? RAYWHITE : ((currentFps > 45) ? YELLOW : RED);
        DrawText(TextFormat("FPS: %.1f", currentFps), x + 10, y + 32, 13, fpsColor);

        if (fpsHistory.size() >= 2) {
            // Find min and max FPS for scaling
            float minFps = 999, maxFps = 0, sum = 0;
            for (float fps : fpsHistory) {
                minFps = std::min(minFps, fps);
                maxFps = std::max(maxFps, fps);
                sum += fps;
            }
            minFps = std::max(0.0f, minFps - 5);
            maxFps = maxFps + 5;
            float avgFps = sum / fpsHistory.size();

            float range = maxFps - minFps;
            if (range < 1) range = 1;

            // Draw graph border
            int graphTop = y + 50;
            int graphHeight = 95;
            DrawRectangleLines(x + 5, graphTop, width - 10, graphHeight, {80, 80, 80, 150});

            // Draw grid lines
            int gridLines = 3;
            for (int i = 0; i <= gridLines; i++) {
                int gridY = graphTop + (graphHeight) * i / gridLines;
                DrawLine(x + 8, gridY, x + width - 8, gridY, {60, 60, 60, 100});
            }

            // Draw FPS line graph
            float pixelWidth = (float)(width - 20) / maxSamples;
            for (size_t i = 0; i < fpsHistory.size() - 1; i++) {
                float fps1 = fpsHistory[i];
                float fps2 = fpsHistory[i + 1];

                float x1 = x + 10 + i * pixelWidth;
                float x2 = x + 10 + (i + 1) * pixelWidth;
                float y1 = graphTop + graphHeight - (fps1 - minFps) / range * graphHeight;
                float y2 = graphTop + graphHeight - (fps2 - minFps) / range * graphHeight;

                Color lineColor = (fps2 > 55) ? RAYWHITE : ((fps2 > 45) ? YELLOW : RED);
                DrawLineEx({x1, y1}, {x2, y2}, 1.5f, lineColor);
            }

            // Draw stats
            DrawText(TextFormat("Max: %.0f", maxFps), x + 10, y + 150, 10, RAYWHITE);
            DrawText(TextFormat("Avg: %.0f", avgFps), x + (width / 2) - 25, y + 150, 10, RAYWHITE);
        }
        
        // Draw sliders below the sidebar
        float sliderSpacing = 36;
        for (size_t i = 0; i < sliders.size(); i++) {
            sliders[i].x = x + 10;
            sliders[i].y = y + 170 + i * sliderSpacing;
            sliders[i].draw();
        }
        
        // Draw checkboxes below the sliders
        int checkboxStartY = y + 170 + sliders.size() * 36 + 20;
        for (size_t i = 0; i < checkboxes.size(); i++) {
            checkboxes[i].x = x + 10;
            checkboxes[i].y = checkboxStartY + i * 25;
            checkboxes[i].draw();
        }
    }

    const BoidParams& getParams() const {
        return params;
    }

private:
    std::vector<Slider> sliders;
    std::vector<Checkbox> checkboxes;
};

#endif
