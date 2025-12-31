#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "raylib.h"
#include <deque>
#include <algorithm>

// FPS Graph tracker for sidebar display
class Sidebar {
public:
    static const int maxSamples = 1080;
    std::deque<float> fpsHistory;

    void update(float fps) {
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > maxSamples) {
            fpsHistory.pop_front();
        }
    }

    void draw(int x, int y, int width, int height) {
        // Background - more transparent
        DrawRectangle(x, y, width, height, {30, 30, 30, 100});
        DrawRectangleLines(x, y, width, height, RAYWHITE);

        // Title
        DrawText("FPS Graph", x + 10, y + 5, 14, RAYWHITE);
        
        // Current FPS display
        float currentFps = fpsHistory.empty() ? 0 : fpsHistory.back();
        DrawText(TextFormat("Current: %.1f", currentFps), x + 10, y + height - 35, 12, RAYWHITE);

        if (fpsHistory.size() < 2) return;

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

        // Draw grid lines
        int gridLines = 4;
        for (int i = 0; i <= gridLines; i++) {
            int gridY = y + 25 + (height - 35) * i / gridLines;
            DrawLine(x + 5, gridY, x + width - 5, gridY, {80, 80, 80, 200});
        }

        // Draw FPS line graph
        float pixelWidth = (float)(width - 15) / maxSamples;
        for (size_t i = 0; i < fpsHistory.size() - 1; i++) {
            float fps1 = fpsHistory[i];
            float fps2 = fpsHistory[i + 1];

            float x1 = x + 10 + i * pixelWidth;
            float x2 = x + 10 + (i + 1) * pixelWidth;
            float y1 = y + height - 10 - (fps1 - minFps) / range * (height - 35);
            float y2 = y + height - 10 - (fps2 - minFps) / range * (height - 35);

            Color lineColor = (fps2 > 55) ? GREEN : ((fps2 > 45) ? YELLOW : RED);
            DrawLineEx({x1, y1}, {x2, y2}, 2.0f, lineColor);
        }

        // Draw labels
        DrawText(TextFormat("Max: %.1f", maxFps), x + 10, y + height - 20, 12, RAYWHITE);
        DrawText(TextFormat("Min: %.1f", minFps), x + width - 90, y + height - 20, 12, RAYWHITE);
        DrawText(TextFormat("Avg: %.1f", avgFps), x + (width / 2) - 30, y + height - 20, 12, RAYWHITE);
    }
};

#endif
