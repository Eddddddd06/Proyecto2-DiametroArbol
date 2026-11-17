#pragma once
// ============================================================
//  Visualizer.hpp  –  Aplicación gráfica interactiva (Raylib)
//  CS2023 AED | UTEC 2026-2  |  "Diámetro de un Árbol"
// ============================================================

#include "Tree.hpp"
#include <raylib.h>
#include <vector>
#include <string>
#include <functional>
#include <array>

// ── Pestañas ──────────────────────────────────────────────────
enum class Tab { INTRO = 0, SIMULATOR = 1, EDGE_CASES = 2, COMPLEXITY = 3 };

// ── Instancias de árbol ───────────────────────────────────────
enum class TreeInstance { MAIN_UNWEIGHTED = 0, MAIN_WEIGHTED = 1 };

// ── Casos borde ───────────────────────────────────────────────
enum class EdgeCaseType { EMPTY = 0, SINGLE_NODE = 1, LINEAR = 2, STAR = 3 };

// ── Paleta (funciones inline, evita conflicto con macros Raylib)
namespace Pal {
    inline Color BG()         { return {15,  17,  26,  255}; }
    inline Color PANEL()      { return {24,  27,  40,  255}; }
    inline Color PANEL2()     { return {30,  34,  50,  255}; }
    inline Color BORDER()     { return {59,  66,  82,  255}; }
    inline Color TEXT()       { return {236, 239, 244, 255}; }
    inline Color TEXT_DIM()   { return {143, 153, 175, 255}; }
    inline Color CYAN_C()     { return {136, 192, 208, 255}; } // #88C0D0 cian
    inline Color BLUE_C()     { return {129, 161, 193, 255}; } // #81A1C1 azul
    inline Color AMBER()      { return {235, 203, 139, 255}; } // #EBCB8B
    inline Color GREEN_C()    { return {163, 190, 140, 255}; } // #A3BE8C
    inline Color RED_C()      { return {191, 97,  106, 255}; } // #BF616A
    inline Color STEEL()      { return {94,  129, 172, 255}; } // #5E81AC
    inline Color ORANGE_C()   { return {208, 135, 112, 255}; }
    inline Color UNVIS()      { return {59,  66,  82,  255}; }
    inline Color WHITE_C()    { return {236, 239, 244, 255}; }
    inline Color DARK()       { return {15,  17,  26,  255}; }
}

// ── Layout ────────────────────────────────────────────────────
namespace Lay {
    constexpr int W        = 1280;
    constexpr int H        = 720;
    constexpr int NAV_H    = 50;
    constexpr int LEFT_W   = 230;
    constexpr int RIGHT_W  = 270;
    constexpr int BOT_H    = 110;
    constexpr int CANVAS_X = LEFT_W;
    constexpr int CANVAS_Y = NAV_H;
    constexpr int CANVAS_W = W - LEFT_W - RIGHT_W;  // 780
    constexpr int CANVAS_H = H - NAV_H - BOT_H;     // 560
    constexpr int RIGHT_X  = W - RIGHT_W;
    constexpr int BOT_Y    = H - BOT_H;
    constexpr int NR       = 24;
}

// ── Botón ─────────────────────────────────────────────────────
struct UIBtn {
    Rectangle   rect;
    std::string label;
    std::string shortcut;
    bool        active    = false;
    bool        hovered   = false;
    Color       baseCol   = {24,27,40,255};
    Color       hoverCol  = {59,66,82,255};
    Color       activeCol = {94,129,172,255};
    std::function<void()> onClick;
};

// ── Visual de nodo ────────────────────────────────────────────
struct NodeVis {
    Vector2 pos       = {0,0};
    Color   col       = {59,66,82,255};
    Color   colTgt    = {59,66,82,255};
    float   scale     = 1.0f;
    float   scaleTgt  = 1.0f;
};

// ── Visual de arista ──────────────────────────────────────────
struct EdgeVis {
    Color col     = {59,66,82,255};
    Color colTgt  = {59,66,82,255};
    float progress = 1.0f;
    float progTgt  = 1.0f;
};

// ═══════════════════════════════════════════════════════════════
class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    void init();
    void update();
    void draw();
    bool shouldClose() const;

private:
    // Fuentes
    Font fontReg, fontBold, fontMono;
    bool fontsLoaded = false;
    void loadFonts();
    Font tryLoadFont(int size);

    // Pestaña
    Tab currentTab = Tab::INTRO;

    // Árbol y algoritmo
    Tree tree;
    std::vector<AlgoStep> steps;
    int  stepIdx      = -1;
    bool playing      = false;
    float playTimer   = 0.0f;
    float speed       = 1.0f;
    float stepInterval= 0.75f;
    int   startNode   = 0;

    // Visuals
    std::vector<NodeVis> nodeVis;
    std::vector<EdgeVis> edgeVis;
    std::vector<Vector2> nodePosCanvas;
    float animTime    = 0.0f;

    // Resultado
    Tree::Result result;

    // Interacción
    int  hoveredNode   = -1;
    bool weightedMode  = false;

    // Cola visualizada
    std::vector<int> displayQueue;

    // Tab 3
    EdgeCaseType activeEdgeCase = EdgeCaseType::EMPTY;

    // Botones
    UIBtn btnPlay, btnNext, btnPrev, btnReset;
    UIBtn btnSpeedDown, btnSpeedUp, btnWeighted;
    std::array<UIBtn,4> tabBtns;
    std::array<UIBtn,4> edgeCaseBtns;

    // Texto barra inferior
    std::string descText = "Presiona [Espacio] o [ Play ] para iniciar la simulacion.";

    // Carga de árboles
    void loadTree(TreeInstance inst);
    void loadEdgeCase(EdgeCaseType ec);
    void computeNodeLayout();
    void initNodeEdgeVis();
    void resetSimulation();

    // Input
    void handleInput();
    void handleTabKeys();
    void handlePlaybackKeys();
    int  getNodeAtMouse(Vector2 mp) const;

    // Pasos
    void goToStep(int idx);
    void stepForward();
    void stepBack();

    // Draw – secciones
    void drawNavBar();
    void drawLeftPanel();
    void drawCanvas();
    void drawRightPanel();
    void drawBottomBar();

    // Draw – pestañas
    void drawTabIntro();
    void drawTabSimulator();
    void drawTabEdgeCases();
    void drawTabComplexity();

    // Draw – árbol
    void drawTree();
    void drawSingleNode(int idx);
    void drawSingleEdge(int eIdx);
    void drawQueuePanel(Rectangle area);
    void drawDistTable(Rectangle area);
    void drawSummaryCards(Rectangle area);
    void drawLegend(Rectangle area);

    // Draw – utilidades
    void drawBtn(UIBtn& btn);
    void drawPanel(Rectangle r, Color fill, Color border, float rad = 8.0f);
    void drawTextC(const char* txt, int cx, int cy, int sz, Color c);
    void drawTextEx2(const char* txt, float x, float y, float sz, float sp, Color c, Font f);
    void drawLabel(const char* txt, int x, int y, int sz, Color c);

    // Helpers
    Color nodeStateColor(NodeState s) const;
    Color edgeStateColor(EdgeState s) const;
    Color lerpC(Color a, Color b, float t) const;
    float easeIO(float t) const;
    std::string distStr(int d) const;
    bool inRect(Rectangle r, Vector2 p) const;
    bool inCircle(Vector2 center, float r, Vector2 p) const;
};
