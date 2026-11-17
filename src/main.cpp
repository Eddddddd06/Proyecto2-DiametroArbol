// ============================================================
//  main.cpp  –  Punto de entrada
//  CS2023 AED | UTEC 2026-2  |  "Diámetro de un Árbol"
// ============================================================
#include "Visualizer.hpp"
#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(Lay::W, Lay::H, "Diámetro de un Árbol  –  CS2023 AED  |  UTEC 2026-2");
    SetTargetFPS(60);
    SetWindowMinSize(1024, 600);
    SetExitKey(KEY_ESCAPE);

    Visualizer app;
    app.init();

    while (!app.shouldClose()) {
        app.update();
        BeginDrawing();
        app.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
