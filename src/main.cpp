#include "Visualizer.h"
#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(Lay::W, Lay::H, "Diametro de un Arbol - AED UTEC 2026-2");
    SetTargetFPS(60);
    SetWindowMinSize(1024, 600);
    SetExitKey(KEY_ESCAPE);

    Viz app;
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
