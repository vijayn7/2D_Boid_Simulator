#include "raylib.h"

int main() {
    const int W = 1000;
    const int H = 700;

    InitWindow(W, H, "Boids - raylib");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("raylib works", 20, 20, 30, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
