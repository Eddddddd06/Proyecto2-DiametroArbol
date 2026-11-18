#include "Visualizer.h"
#include <raylib.h>

int main() {
    SetConfigFlags( FLAG_MSAA_4X_HINT|FLAG_VSYNC_HINT);
    InitWindow(Lay::W ,Lay::H ,"Diametro de un Arbol  -  AED UTEC 2026-2" );
    SetTargetFPS(60 );
    SetExitKey(KEY_ESCAPE);

    int mon= GetCurrentMonitor();
    int monW =GetMonitorWidth(mon);
    int monH = GetMonitorHeight(mon);
    SetWindowPosition((monW - Lay::W)/2 , (monH-Lay::H)/2);
    {
        Viz app;
        app.init();
        while (!app.shouldClose()) {
            app.update();
            BeginDrawing();
            app.draw();
            EndDrawing();
        }

    }
    CloseWindow();
    return 0;
}
