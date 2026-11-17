// ============================================================
//  Visualizer.cpp  –  Implementación gráfica con Raylib
//  CS2023 AED | UTEC 2026-2  |  "Diámetro de un Árbol"
// ============================================================

#include "Visualizer.hpp"
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <queue>
#include <string>
#include <vector>
#include <cstring>

using namespace Lay;

// ════════════════════════════════════════════════════════════
//  Constructor / Destructor
// ════════════════════════════════════════════════════════════

Visualizer::Visualizer() {}

Visualizer::~Visualizer() {
    if (fontsLoaded) {
        // Only unload if they aren't the default font
        // GetFontDefault() returns a font with texture.id == 1; we avoid unloading it
        if (fontReg.texture.id  > 1) UnloadFont(fontReg);
        if (fontBold.texture.id > 1) UnloadFont(fontBold);
        if (fontMono.texture.id > 1) UnloadFont(fontMono);
    }
}

// ════════════════════════════════════════════════════════════
//  Font helpers
// ════════════════════════════════════════════════════════════

Font Visualizer::tryLoadFont(int size) {
    const char* paths[] = {
        "/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/Hack-Regular.ttf",
        "/usr/share/fonts/nerd-fonts-complete/TTF/JetBrainsMono-Regular.ttf"
    };
    for (auto& p : paths) {
        if (FileExists(p)) {
            Font f = LoadFontEx(p, size, nullptr, 512);
            if (f.texture.id > 0) return f;
        }
    }
    return GetFontDefault();
}

void Visualizer::loadFonts() {
    fontReg  = tryLoadFont(18);
    fontBold = tryLoadFont(20);
    fontMono = tryLoadFont(16);
    fontsLoaded = true;
}

// ════════════════════════════════════════════════════════════
//  init()
// ════════════════════════════════════════════════════════════

void Visualizer::init() {
    loadFonts();

    // ── Tab buttons ──────────────────────────────────────────
    tabBtns[0].rect = {5, 2, 310, 46};
    tabBtns[0].label = "F1  Portada & Intro";
    tabBtns[0].shortcut = "";
    tabBtns[0].onClick = [this]{ currentTab = Tab::INTRO; };

    tabBtns[1].rect = {320, 2, 310, 46};
    tabBtns[1].label = "F2  Simulador Principal";
    tabBtns[1].shortcut = "";
    tabBtns[1].onClick = [this]{ currentTab = Tab::SIMULATOR; };

    tabBtns[2].rect = {635, 2, 310, 46};
    tabBtns[2].label = "F3  Casos Borde";
    tabBtns[2].shortcut = "";
    tabBtns[2].onClick = [this]{ currentTab = Tab::EDGE_CASES; };

    tabBtns[3].rect = {950, 2, 320, 46};
    tabBtns[3].label = "F4  Complejidad & Créditos";
    tabBtns[3].shortcut = "";
    tabBtns[3].onClick = [this]{ currentTab = Tab::COMPLEXITY; };

    // ── Control buttons ──────────────────────────────────────
    btnPlay.rect     = {5, 70, 220, 38};
    btnPlay.label    = "▶  Play / ⏸ Pausa";
    btnPlay.shortcut = "[Espacio]";
    btnPlay.onClick  = [this]{
        playing = !playing;
        if (playing && stepIdx == -1) stepForward();
    };

    btnNext.rect     = {5, 114, 107, 34};
    btnNext.label    = "⏭ Siguiente";
    btnNext.shortcut = "[→]";
    btnNext.onClick  = [this]{ stepForward(); };

    btnPrev.rect     = {118, 114, 107, 34};
    btnPrev.label    = "⏮ Anterior";
    btnPrev.shortcut = "[←]";
    btnPrev.onClick  = [this]{ stepBack(); };

    btnReset.rect     = {5, 154, 220, 34};
    btnReset.label    = "🔄 Reiniciar";
    btnReset.shortcut = "[R]";
    btnReset.onClick  = [this]{ resetSimulation(); };

    btnSpeedDown.rect     = {5, 194, 105, 30};
    btnSpeedDown.label    = "Velocidad −";
    btnSpeedDown.shortcut = "[-]";
    btnSpeedDown.onClick  = [this]{ speed = std::max(0.25f, speed - 0.25f); };

    btnSpeedUp.rect     = {116, 194, 109, 30};
    btnSpeedUp.label    = "Velocidad +";
    btnSpeedUp.shortcut = "[+]";
    btnSpeedUp.onClick  = [this]{ speed = std::min(3.0f, speed + 0.25f); };

    btnWeighted.rect     = {5, 230, 220, 30};
    btnWeighted.label    = "Alternar Árbol Ponderado";
    btnWeighted.shortcut = "[W]";
    btnWeighted.onClick  = [this]{
        weightedMode = !weightedMode;
        if (weightedMode) loadTree(TreeInstance::MAIN_WEIGHTED);
        else              loadTree(TreeInstance::MAIN_UNWEIGHTED);
    };

    // ── Edge case buttons ────────────────────────────────────
    edgeCaseBtns[0].rect  = {5, 280, 220, 36};
    edgeCaseBtns[0].label = "Caso 1: Vacío (N=0)";
    edgeCaseBtns[0].shortcut = "";
    edgeCaseBtns[0].onClick = [this]{
        loadEdgeCase(EdgeCaseType::EMPTY);
        activeEdgeCase = EdgeCaseType::EMPTY;
    };

    edgeCaseBtns[1].rect  = {5, 322, 220, 36};
    edgeCaseBtns[1].label = "Caso 2: Un Nodo (N=1)";
    edgeCaseBtns[1].shortcut = "";
    edgeCaseBtns[1].onClick = [this]{
        loadEdgeCase(EdgeCaseType::SINGLE_NODE);
        activeEdgeCase = EdgeCaseType::SINGLE_NODE;
    };

    edgeCaseBtns[2].rect  = {5, 364, 220, 36};
    edgeCaseBtns[2].label = "Caso 3: Cadena N=6";
    edgeCaseBtns[2].shortcut = "";
    edgeCaseBtns[2].onClick = [this]{
        loadEdgeCase(EdgeCaseType::LINEAR);
        activeEdgeCase = EdgeCaseType::LINEAR;
    };

    edgeCaseBtns[3].rect  = {5, 406, 220, 36};
    edgeCaseBtns[3].label = "Caso 4: Estrella N=7";
    edgeCaseBtns[3].shortcut = "";
    edgeCaseBtns[3].onClick = [this]{
        loadEdgeCase(EdgeCaseType::STAR);
        activeEdgeCase = EdgeCaseType::STAR;
    };

    // ── Default tree ─────────────────────────────────────────
    loadTree(TreeInstance::MAIN_UNWEIGHTED);
}

// ════════════════════════════════════════════════════════════
//  loadTree / loadEdgeCase
// ════════════════════════════════════════════════════════════

void Visualizer::loadTree(TreeInstance inst) {
    tree.resize(10);
    if (inst == TreeInstance::MAIN_UNWEIGHTED) {
        tree.addEdge(0, 1, 1); tree.addEdge(0, 2, 1); tree.addEdge(0, 3, 1);
        tree.addEdge(1, 4, 1); tree.addEdge(1, 5, 1);
        tree.addEdge(2, 6, 1);
        tree.addEdge(3, 7, 1);
        tree.addEdge(4, 8, 1);
        tree.addEdge(7, 9, 1);
    } else {
        tree.addEdge(0, 1, 3); tree.addEdge(0, 2, 1); tree.addEdge(0, 3, 4);
        tree.addEdge(1, 4, 2); tree.addEdge(1, 5, 5);
        tree.addEdge(2, 6, 1);
        tree.addEdge(3, 7, 3);
        tree.addEdge(4, 8, 6);
        tree.addEdge(7, 9, 2);
    }
    computeNodeLayout();
    initNodeEdgeVis();
    resetSimulation();
}

void Visualizer::loadEdgeCase(EdgeCaseType ec) {
    switch (ec) {
        case EdgeCaseType::EMPTY:
            tree.clear();
            nodeVis.clear();
            edgeVis.clear();
            resetSimulation();
            break;
        case EdgeCaseType::SINGLE_NODE:
            tree.resize(1);
            computeNodeLayout();
            initNodeEdgeVis();
            resetSimulation();
            break;
        case EdgeCaseType::LINEAR:
            tree.resize(6);
            tree.addEdge(0, 1, 1); tree.addEdge(1, 2, 1); tree.addEdge(2, 3, 1);
            tree.addEdge(3, 4, 1); tree.addEdge(4, 5, 1);
            computeNodeLayout();
            initNodeEdgeVis();
            resetSimulation();
            break;
        case EdgeCaseType::STAR:
            tree.resize(7);
            tree.addEdge(0, 1, 1); tree.addEdge(0, 2, 1); tree.addEdge(0, 3, 1);
            tree.addEdge(0, 4, 1); tree.addEdge(0, 5, 1); tree.addEdge(0, 6, 1);
            computeNodeLayout();
            initNodeEdgeVis();
            resetSimulation();
            break;
    }
}

// ════════════════════════════════════════════════════════════
//  computeNodeLayout()
// ════════════════════════════════════════════════════════════

void Visualizer::computeNodeLayout() {
    int n = tree.n;
    nodePosCanvas.clear();
    nodePosCanvas.resize(n);

    if (n == 0) return;
    if (n == 1) {
        nodePosCanvas[0] = {390.0f, 280.0f};
        return;
    }

    // BFS to get depths and levels
    std::vector<int> depth(n, -1);
    std::vector<std::vector<int>> byDepth;
    std::queue<int> bfsQ;
    depth[0] = 0;
    bfsQ.push(0);
    byDepth.push_back({0});

    while (!bfsQ.empty()) {
        int u = bfsQ.front(); bfsQ.pop();
        for (auto& e : tree.adj[u]) {
            int v = e.to;
            if (depth[v] == -1) {
                depth[v] = depth[u] + 1;
                bfsQ.push(v);
                while ((int)byDepth.size() <= depth[v])
                    byDepth.push_back({});
                byDepth[depth[v]].push_back(v);
            }
        }
    }

    int maxDepth = (int)byDepth.size() - 1;

    // Detect LINEAR: every level has exactly 1 node, OR maxDepth >= n-1
    bool isLinear = (maxDepth >= n - 1);
    if (!isLinear) {
        isLinear = true;
        for (auto& lvl : byDepth) {
            if ((int)lvl.size() != 1) { isLinear = false; break; }
        }
    }

    // Detect STAR: n>1, maxDepth==1 and all adj of 0 are leaves
    bool isStar = false;
    if (n > 1 && maxDepth == 1) {
        isStar = true;
        for (auto& e : tree.adj[0]) {
            int v = e.to;
            if ((int)tree.adj[v].size() != 1) { isStar = false; break; }
        }
    }

    const float CW = (float)CANVAS_W;
    const float CH = (float)CANVAS_H;

    if (isLinear) {
        float padding = 60.0f;
        float step = (n > 1) ? (CW - 2.0f * padding) / (float)(n - 1) : 0.0f;
        float y = CH / 2.0f;
        // BFS order
        std::vector<int> order;
        {
            std::queue<int> q2;
            std::vector<bool> vis(n, false);
            q2.push(0); vis[0] = true;
            while (!q2.empty()) {
                int u = q2.front(); q2.pop();
                order.push_back(u);
                for (auto& e : tree.adj[u]) {
                    if (!vis[e.to]) { vis[e.to] = true; q2.push(e.to); }
                }
            }
        }
        for (int i = 0; i < n; i++) {
            nodePosCanvas[order[i]] = {padding + i * step, y};
        }
    } else if (isStar) {
        nodePosCanvas[0] = {390.0f, 280.0f};
        int leaves = (int)tree.adj[0].size();
        float radius = 200.0f;
        for (int i = 0; i < leaves; i++) {
            float angle = -M_PI / 2.0f + 2.0f * M_PI * i / (float)leaves;
            nodePosCanvas[tree.adj[0][i].to] = {
                390.0f + radius * std::cos(angle),
                280.0f + radius * std::sin(angle)
            };
        }
    } else {
        // General hierarchical layout
        float topPad   = 70.0f;
        float botPad   = 50.0f;
        float leftPad  = 70.0f;
        float rightPad = 70.0f;
        float levelH   = (CH - topPad - botPad) / (float)std::max(1, maxDepth);

        for (int d = 0; d <= maxDepth; d++) {
            auto& lvl = byDepth[d];
            int cnt   = (int)lvl.size();
            float y   = topPad + d * levelH;
            if (cnt == 1) {
                nodePosCanvas[lvl[0]] = {CW / 2.0f, y};
            } else {
                float step = (CW - leftPad - rightPad) / (float)(cnt - 1);
                for (int i = 0; i < cnt; i++) {
                    nodePosCanvas[lvl[i]] = {leftPad + i * step, y};
                }
            }
        }
    }
}

// ════════════════════════════════════════════════════════════
//  initNodeEdgeVis / resetSimulation
// ════════════════════════════════════════════════════════════

void Visualizer::initNodeEdgeVis() {
    int n = tree.n;
    nodeVis.resize(n);
    for (int i = 0; i < n; i++) {
        nodeVis[i].pos     = (n > 0 && i < (int)nodePosCanvas.size())
                              ? nodePosCanvas[i] : Vector2{390, 280};
        nodeVis[i].col     = Pal::UNVIS();
        nodeVis[i].colTgt  = Pal::UNVIS();
        nodeVis[i].scale   = 1.0f;
        nodeVis[i].scaleTgt= 1.0f;
    }
    int E = (int)tree.edgeList.size();
    edgeVis.resize(E);
    for (int i = 0; i < E; i++) {
        edgeVis[i].col      = Pal::BORDER();
        edgeVis[i].colTgt   = Pal::BORDER();
        edgeVis[i].progress = 1.0f;
        edgeVis[i].progTgt  = 1.0f;
    }
}

void Visualizer::resetSimulation() {
    stepIdx    = -1;
    playing    = false;
    playTimer  = 0.0f;
    steps.clear();
    result     = Tree::Result{};
    descText   = "Presiona [Espacio] o [ ▶ Play ] para iniciar la simulación.";
    displayQueue.clear();
    initNodeEdgeVis();
}

// ════════════════════════════════════════════════════════════
//  Step navigation
// ════════════════════════════════════════════════════════════

void Visualizer::goToStep(int idx) {
    if (steps.empty() || idx < 0 || idx >= (int)steps.size()) return;
    stepIdx = idx;
    const AlgoStep& s = steps[idx];
    int n = tree.n;
    for (int i = 0; i < n; i++) {
        nodeVis[i].colTgt = nodeStateColor(s.nodeStates[i]);
        if (s.nodeStates[i] == NodeState::CURRENT || s.nodeStates[i] == NodeState::ON_DIAMETER)
            nodeVis[i].scaleTgt = 1.15f;
        else
            nodeVis[i].scaleTgt = 1.0f;
    }
    int E = (int)tree.edgeList.size();
    for (int i = 0; i < E; i++) {
        edgeVis[i].colTgt = edgeStateColor(s.edgeStates[i]);
    }
    displayQueue = s.queueState;
    descText     = s.description;
}

void Visualizer::stepForward() {
    if (steps.empty()) {
        steps  = tree.computeSteps(startNode);
        result = Tree::extractResult(steps);
    }
    if (stepIdx + 1 < (int)steps.size()) {
        goToStep(stepIdx + 1);
    } else {
        playing = false;
    }
}

void Visualizer::stepBack() {
    if (stepIdx > 0) {
        goToStep(stepIdx - 1);
    } else if (stepIdx == 0) {
        resetSimulation();
    }
}

// ════════════════════════════════════════════════════════════
//  Input
// ════════════════════════════════════════════════════════════

int Visualizer::getNodeAtMouse(Vector2 mp) const {
    int n = tree.n;
    for (int i = 0; i < n; i++) {
        Vector2 wp = {nodeVis[i].pos.x + (float)LEFT_W,
                      nodeVis[i].pos.y + (float)NAV_H};
        if (inCircle(wp, (float)(NR + 4), mp)) return i;
    }
    return -1;
}

void Visualizer::handleTabKeys() {
    if (IsKeyPressed(KEY_F1)) currentTab = Tab::INTRO;
    if (IsKeyPressed(KEY_F2)) currentTab = Tab::SIMULATOR;
    if (IsKeyPressed(KEY_F3)) currentTab = Tab::EDGE_CASES;
    if (IsKeyPressed(KEY_F4)) currentTab = Tab::COMPLEXITY;
}

void Visualizer::handlePlaybackKeys() {
    if (IsKeyPressed(KEY_SPACE))  btnPlay.onClick();
    if (IsKeyPressed(KEY_RIGHT))  stepForward();
    if (IsKeyPressed(KEY_LEFT))   stepBack();
    if (IsKeyPressed(KEY_R))      resetSimulation();
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        speed = std::max(0.25f, speed - 0.25f);
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        speed = std::min(3.0f, speed + 0.25f);
    if (IsKeyPressed(KEY_W)) btnWeighted.onClick();
}

void Visualizer::handleInput() {
    Vector2 mp = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Tab buttons (always active)
    for (auto& btn : tabBtns) {
        if (clicked && inRect(btn.rect, mp)) { btn.onClick(); return; }
    }

    // Control buttons
    UIBtn* controlBtns[] = {
        &btnPlay, &btnNext, &btnPrev, &btnReset,
        &btnSpeedDown, &btnSpeedUp, &btnWeighted
    };
    for (auto* btn : controlBtns) {
        if (clicked && inRect(btn->rect, mp)) { btn->onClick(); return; }
    }

    // Edge case buttons only on EDGE_CASES tab
    if (currentTab == Tab::EDGE_CASES) {
        for (auto& btn : edgeCaseBtns) {
            if (clicked && inRect(btn.rect, mp)) { btn.onClick(); return; }
        }
    }

    // Node click to change startNode
    if (currentTab == Tab::SIMULATOR || currentTab == Tab::EDGE_CASES) {
        if (clicked) {
            int nodeIdx = getNodeAtMouse(mp);
            if (nodeIdx >= 0) {
                startNode = nodeIdx;
                resetSimulation();
                return;
            }
        }
    }

    // Keyboard
    handleTabKeys();
    handlePlaybackKeys();
}

// ════════════════════════════════════════════════════════════
//  update()
// ════════════════════════════════════════════════════════════

void Visualizer::update() {
    float dt = GetFrameTime();
    animTime += dt;

    if (playing) {
        playTimer += dt;
        if (playTimer >= stepInterval / speed) {
            stepForward();
            playTimer = 0.0f;
        }
    }

    // Smooth color/scale lerp
    for (auto& nv : nodeVis) {
        nv.col   = lerpC(nv.col,   nv.colTgt,   0.12f);
        nv.scale = nv.scale + (nv.scaleTgt - nv.scale) * 0.12f;
    }
    for (auto& ev : edgeVis) {
        ev.col = lerpC(ev.col, ev.colTgt, 0.12f);
    }

    hoveredNode = getNodeAtMouse(GetMousePosition());

    handleInput();
}

// ════════════════════════════════════════════════════════════
//  shouldClose()
// ════════════════════════════════════════════════════════════

bool Visualizer::shouldClose() const {
    return WindowShouldClose();
}

// ════════════════════════════════════════════════════════════
//  Helper draw utilities
// ════════════════════════════════════════════════════════════

void Visualizer::drawPanel(Rectangle r, Color fill, Color border, float rad) {
    float ratio = rad / std::max(r.width, r.height);
    DrawRectangleRounded(r, ratio, 6, fill);
    if (border.a > 0) {
        DrawRectangleRoundedLines(r, ratio, 6, border);
    }
}

void Visualizer::drawTextC(const char* txt, int cx, int cy, int sz, Color c) {
    int w = MeasureText(txt, sz);
    DrawText(txt, cx - w / 2, cy - sz / 2, sz, c);
}

void Visualizer::drawTextEx2(const char* txt, float x, float y, float sz, float sp, Color c, Font f) {
    DrawTextEx(f, txt, {x, y}, sz, sp, c);
}

void Visualizer::drawLabel(const char* txt, int x, int y, int sz, Color c) {
    DrawText(txt, x, y, sz, c);
}

void Visualizer::drawBtn(UIBtn& btn) {
    btn.hovered = inRect(btn.rect, GetMousePosition());
    Color bg = btn.active  ? Pal::STEEL()  :
               btn.hovered ? Pal::BORDER() : Pal::PANEL2();
    DrawRectangleRounded(btn.rect, 0.3f, 6, bg);
    DrawRectangleRoundedLines(btn.rect, 0.3f, 6, Pal::BORDER());

    // Label + shortcut centered
    int bx = (int)(btn.rect.x + btn.rect.width  / 2);
    int by = (int)(btn.rect.y + btn.rect.height  / 2);

    // If there's a shortcut, place label slightly above and shortcut below
    if (!btn.shortcut.empty()) {
        int lsz = 13;
        int ssz = 10;
        int lw  = MeasureText(btn.label.c_str(), lsz);
        int sw  = MeasureText(btn.shortcut.c_str(), ssz);
        DrawText(btn.label.c_str(),    bx - lw / 2, by - lsz, lsz, Pal::TEXT());
        DrawText(btn.shortcut.c_str(), bx - sw / 2, by + 1,   ssz, Pal::TEXT_DIM());
    } else {
        int lsz = 13;
        int lw  = MeasureText(btn.label.c_str(), lsz);
        DrawText(btn.label.c_str(), bx - lw / 2, by - lsz / 2, lsz, Pal::TEXT());
    }
}

// ════════════════════════════════════════════════════════════
//  Helpers matemáticos
// ════════════════════════════════════════════════════════════

Color Visualizer::nodeStateColor(NodeState s) const {
    switch (s) {
        case NodeState::UNVISITED:   return Pal::UNVIS();
        case NodeState::IN_QUEUE:    return Pal::AMBER();
        case NodeState::CURRENT:     return Pal::CYAN_C();
        case NodeState::PROCESSED:   return Pal::STEEL();
        case NodeState::EXTREMO_A:   return Pal::RED_C();
        case NodeState::EXTREMO_B:   return Pal::RED_C();
        case NodeState::ON_DIAMETER: return Pal::GREEN_C();
        default:                     return Pal::UNVIS();
    }
}

Color Visualizer::edgeStateColor(EdgeState s) const {
    switch (s) {
        case EdgeState::UNVISITED:   return {40, 45, 65, 255};
        case EdgeState::TREE_EDGE:   return {94, 129, 172, 200};
        case EdgeState::ON_DIAMETER: return Pal::GREEN_C();
        default:                     return {40, 45, 65, 255};
    }
}

Color Visualizer::lerpC(Color a, Color b, float t) const {
    t = std::clamp(t, 0.0f, 1.0f);
    return Color{
        (unsigned char)(a.r + (b.r - a.r) * t),
        (unsigned char)(a.g + (b.g - a.g) * t),
        (unsigned char)(a.b + (b.b - a.b) * t),
        (unsigned char)(a.a + (b.a - a.a) * t)
    };
}

float Visualizer::easeIO(float t) const {
    return 3.0f * t * t - 2.0f * t * t * t;
}

std::string Visualizer::distStr(int d) const {
    if (d == -1) return "\xe2\x88\x9e"; // UTF-8 infinity ∞
    return std::to_string(d);
}

bool Visualizer::inRect(Rectangle r, Vector2 p) const {
    return CheckCollisionPointRec(p, r);
}

bool Visualizer::inCircle(Vector2 center, float r, Vector2 p) const {
    return Vector2Distance(p, center) <= r;
}

// ════════════════════════════════════════════════════════════
//  draw()
// ════════════════════════════════════════════════════════════

void Visualizer::draw() {
    ClearBackground(Pal::BG());
    drawNavBar();

    switch (currentTab) {
        case Tab::INTRO:
            drawTabIntro();
            break;
        case Tab::SIMULATOR:
            drawLeftPanel();
            drawCanvas();
            drawRightPanel();
            drawBottomBar();
            break;
        case Tab::EDGE_CASES:
            drawLeftPanel();
            drawCanvas();
            drawRightPanel();
            drawBottomBar();
            break;
        case Tab::COMPLEXITY:
            drawTabComplexity();
            break;
    }
}

// ════════════════════════════════════════════════════════════
//  drawNavBar()
// ════════════════════════════════════════════════════════════

void Visualizer::drawNavBar() {
    DrawRectangle(0, 0, W, NAV_H, Pal::PANEL());
    DrawLine(0, NAV_H, W, NAV_H, Pal::BORDER());

    for (int i = 0; i < 4; i++) {
        bool isActive = ((int)currentTab == i);
        tabBtns[i].active = isActive;
        drawBtn(tabBtns[i]);
    }
}

// ════════════════════════════════════════════════════════════
//  drawLeftPanel()
// ════════════════════════════════════════════════════════════

void Visualizer::drawLeftPanel() {
    DrawRectangle(0, NAV_H, LEFT_W, H - NAV_H, Pal::PANEL());
    DrawLine(LEFT_W, NAV_H, LEFT_W, H, Pal::BORDER());

    // Section title
    drawLabel("Controles", 10, NAV_H + 10, 14, Pal::TEXT_DIM());

    // Control buttons
    btnPlay.active  = playing;
    drawBtn(btnPlay);
    drawBtn(btnNext);
    drawBtn(btnPrev);
    drawBtn(btnReset);
    drawBtn(btnSpeedDown);
    drawBtn(btnSpeedUp);
    btnWeighted.active = weightedMode;
    drawBtn(btnWeighted);

    // Speed label
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << "Velocidad: " << speed << "x";
        drawLabel(ss.str().c_str(), 10, 232, 11, Pal::AMBER());
    }

    // Edge case section
    if (currentTab == Tab::EDGE_CASES) {
        DrawLine(5, 263, LEFT_W - 5, 263, Pal::BORDER());
        drawLabel("Casos Borde:", 10, 267, 13, Pal::TEXT_DIM());
        for (int i = 0; i < 4; i++) {
            edgeCaseBtns[i].active = (activeEdgeCase == (EdgeCaseType)i);
            drawBtn(edgeCaseBtns[i]);
        }
    }

    // Start node section
    {
        DrawLine(5, H - NAV_H - 115, LEFT_W - 5, H - NAV_H - 115, Pal::BORDER());
        drawLabel("Nodo Inicial:", 10, H - NAV_H - 110, 13, Pal::TEXT_DIM());
        std::string startStr = "Inicio: v" + std::to_string(startNode);
        drawLabel(startStr.c_str(), 10, H - NAV_H - 90, 15, Pal::CYAN_C());
        drawLabel("Clic en nodo", 10, H - NAV_H - 68, 11, Pal::TEXT_DIM());
        drawLabel("para cambiar", 10, H - NAV_H - 55, 11, Pal::TEXT_DIM());
    }
}

// ════════════════════════════════════════════════════════════
//  drawCanvas()
// ════════════════════════════════════════════════════════════

void Visualizer::drawCanvas() {
    Rectangle canvasRect = {(float)LEFT_W, (float)NAV_H, (float)CANVAS_W, (float)CANVAS_H};
    drawPanel(canvasRect, Pal::PANEL2(), Pal::BORDER(), 4.0f);

    BeginScissorMode(LEFT_W, NAV_H, CANVAS_W, CANVAS_H);

    if (tree.n == 0) {
        drawTextC("Árbol Vacío (N = 0)", LEFT_W + CANVAS_W / 2, NAV_H + CANVAS_H / 2, 22, Pal::TEXT_DIM());
    } else {
        drawTree();
    }

    // Legend top-right corner of canvas
    drawLegend({(float)(LEFT_W + CANVAS_W - 170), (float)(NAV_H + 5), 165.0f, 130.0f});

    EndScissorMode();
}

// ════════════════════════════════════════════════════════════
//  drawRightPanel()
// ════════════════════════════════════════════════════════════

void Visualizer::drawRightPanel() {
    DrawRectangle(RIGHT_X, NAV_H, RIGHT_W, H - NAV_H, Pal::PANEL());
    DrawLine(RIGHT_X, NAV_H, RIGHT_X, H, Pal::BORDER());

    int panH = H - NAV_H;
    int topH  = (int)(panH * 0.40f);
    int midH  = (int)(panH * 0.35f);
    int botH  = panH - topH - midH;

    Rectangle topArea = {(float)RIGHT_X, (float)NAV_H,       (float)RIGHT_W, (float)topH};
    Rectangle midArea = {(float)RIGHT_X, (float)(NAV_H+topH), (float)RIGHT_W, (float)midH};
    Rectangle botArea = {(float)RIGHT_X, (float)(NAV_H+topH+midH), (float)RIGHT_W, (float)botH};

    drawQueuePanel(topArea);
    drawDistTable(midArea);
    drawSummaryCards(botArea);
}

// ════════════════════════════════════════════════════════════
//  drawBottomBar()
// ════════════════════════════════════════════════════════════

void Visualizer::drawBottomBar() {
    DrawRectangle(LEFT_W, BOT_Y, CANVAS_W, BOT_H, {18, 20, 32, 255});
    DrawLine(LEFT_W, BOT_Y, LEFT_W + CANVAS_W, BOT_Y, Pal::BORDER());

    // Word-wrap the description text
    std::string text = descText;
    const int maxChars = 95;
    int x = LEFT_W + 10;
    int y = BOT_Y + 10;
    int lineH = 17;

    while (!text.empty()) {
        if ((int)text.size() <= maxChars) {
            DrawText(text.c_str(), x, y, 15, Pal::TEXT());
            break;
        }
        // Find last space before maxChars
        size_t cut = maxChars;
        size_t sp  = text.rfind(' ', cut);
        if (sp == std::string::npos) sp = cut;
        DrawText(text.substr(0, sp).c_str(), x, y, 15, Pal::TEXT());
        text = text.substr(sp + 1);
        y += lineH;
        if (y > BOT_Y + BOT_H - lineH) break;
    }
}

// ════════════════════════════════════════════════════════════
//  drawTree()
// ════════════════════════════════════════════════════════════

void Visualizer::drawTree() {
    int E = (int)tree.edgeList.size();
    for (int i = 0; i < E; i++)
        drawSingleEdge(i);
    for (int i = 0; i < tree.n; i++)
        drawSingleNode(i);
}

void Visualizer::drawSingleEdge(int eIdx) {
    auto [u, v] = tree.edgeList[eIdx];
    if (u >= tree.n || v >= tree.n) return;

    Vector2 p1 = nodeVis[u].pos;
    Vector2 p2 = nodeVis[v].pos;
    Color c    = edgeVis[eIdx].col;

    float thickness = 2.5f;
    bool onDiam = (edgeVis[eIdx].colTgt.r == Pal::GREEN_C().r &&
                   edgeVis[eIdx].colTgt.g == Pal::GREEN_C().g &&
                   edgeVis[eIdx].colTgt.b == Pal::GREEN_C().b);
    if (onDiam) thickness = 4.5f;

    Vector2 wp1 = {p1.x + (float)LEFT_W, p1.y + (float)NAV_H};
    Vector2 wp2 = {p2.x + (float)LEFT_W, p2.y + (float)NAV_H};

    // Glow for ON_DIAMETER
    if (onDiam) {
        Color glow = {Pal::GREEN_C().r, Pal::GREEN_C().g, Pal::GREEN_C().b, 60};
        DrawLineEx(wp1, wp2, thickness + 6.0f, glow);
    }

    DrawLineEx(wp1, wp2, thickness, c);

    // Weight label for weighted mode
    if (weightedMode && eIdx < (int)tree.edgeWeights.size()) {
        int w = tree.edgeWeights[eIdx];
        Vector2 mid = {(wp1.x + wp2.x) / 2.0f, (wp1.y + wp2.y) / 2.0f};
        std::string ws = std::to_string(w);
        int tsz = 12;
        int tw  = MeasureText(ws.c_str(), tsz);
        DrawRectangle((int)mid.x - tw/2 - 2, (int)mid.y - tsz/2 - 1, tw + 4, tsz + 2, {18, 20, 32, 200});
        DrawText(ws.c_str(), (int)mid.x - tw/2, (int)mid.y - tsz/2, tsz, Pal::TEXT_DIM());
    }
}

void Visualizer::drawSingleNode(int idx) {
    if (idx >= (int)nodeVis.size()) return;
    Vector2 wp = {nodeVis[idx].pos.x + (float)LEFT_W,
                  nodeVis[idx].pos.y + (float)NAV_H};
    Color c    = nodeVis[idx].col;
    float s    = nodeVis[idx].scale;
    int r      = (int)(NR * s);

    // Halo for hovered
    if (idx == hoveredNode) {
        DrawCircle((int)wp.x, (int)wp.y, r + 6, {Pal::TEXT().r, Pal::TEXT().g, Pal::TEXT().b, 40});
    }

    // Halo for start node while idle
    if (idx == startNode && stepIdx == -1) {
        DrawCircle((int)wp.x, (int)wp.y, r + 8,
                   {Pal::AMBER().r, Pal::AMBER().g, Pal::AMBER().b, 60});
    }

    // Pulse for CURRENT / ON_DIAMETER
    if (idx < (int)steps.size() == false) { /* skip */ }
    if (stepIdx >= 0 && stepIdx < (int)steps.size()) {
        NodeState ns = steps[stepIdx].nodeStates[idx];
        if (ns == NodeState::CURRENT || ns == NodeState::ON_DIAMETER) {
            float pulse = 0.5f + 0.5f * std::sin(animTime * 5.0f);
            DrawCircle((int)wp.x, (int)wp.y, r + 4 + (int)(pulse * 3),
                       {c.r, c.g, c.b, 50});
        }
    }

    DrawCircle((int)wp.x, (int)wp.y, r, c);
    DrawCircleLines((int)wp.x, (int)wp.y, (float)r, {Pal::TEXT().r, Pal::TEXT().g, Pal::TEXT().b, 180});

    // Label "v{idx}"
    std::string lbl = "v" + std::to_string(idx);
    // Decide text color based on background brightness
    int bright = (int)c.r + c.g + c.b;
    Color textCol = (bright > 420) ? Pal::DARK() : Pal::TEXT();
    int tsz = 11;
    int tw  = MeasureText(lbl.c_str(), tsz);
    DrawText(lbl.c_str(), (int)wp.x - tw/2, (int)wp.y - tsz/2, tsz, textCol);

    // Distance pastille below node
    if (stepIdx >= 0 && stepIdx < (int)steps.size() &&
        idx < (int)steps[stepIdx].distArr.size()) {
        int d = steps[stepIdx].distArr[idx];
        std::string ds = "d=" + distStr(d);
        int dsz = 10;
        int dw  = MeasureText(ds.c_str(), dsz);
        int px  = (int)wp.x - dw/2 - 2;
        int py  = (int)wp.y + r + 2;
        DrawRectangle(px, py, dw + 4, dsz + 2, {30, 34, 50, 200});
        DrawText(ds.c_str(), px + 2, py + 1, dsz, Pal::TEXT_DIM());
    }
}

// ════════════════════════════════════════════════════════════
//  drawQueuePanel()
// ════════════════════════════════════════════════════════════

void Visualizer::drawQueuePanel(Rectangle area) {
    DrawLine((int)area.x, (int)(area.y + area.height),
             (int)(area.x + area.width), (int)(area.y + area.height), Pal::BORDER());

    int tx = (int)area.x + 8;
    int ty = (int)area.y + 6;
    DrawText("Cola BFS", tx, ty, 13, Pal::TEXT_DIM());
    ty += 20;

    if (stepIdx < 0) {
        DrawText("[ esperando ]", tx, ty, 12, Pal::TEXT_DIM());
        return;
    }

    if (displayQueue.empty()) {
        DrawText("[ Vacia ]", tx, ty, 12, Pal::TEXT_DIM());
        return;
    }

    int bw = 32, bh = 22, gap = 4;
    int x = tx;
    for (int node : displayQueue) {
        if (x + bw > (int)(area.x + area.width) - 4) {
            x = tx;
            ty += bh + gap;
        }
        DrawRectangle(x, ty, bw, bh, {Pal::AMBER().r, Pal::AMBER().g, Pal::AMBER().b, 160});
        DrawRectangleLines(x, ty, bw, bh, Pal::BORDER());
        std::string lbl = "v" + std::to_string(node);
        int lw = MeasureText(lbl.c_str(), 11);
        DrawText(lbl.c_str(), x + bw/2 - lw/2, ty + bh/2 - 5, 11, Pal::DARK());
        x += bw + gap;
    }
}

// ════════════════════════════════════════════════════════════
//  drawDistTable()
// ════════════════════════════════════════════════════════════

void Visualizer::drawDistTable(Rectangle area) {
    DrawLine((int)area.x, (int)(area.y + area.height),
             (int)(area.x + area.width), (int)(area.y + area.height), Pal::BORDER());

    int tx = (int)area.x + 8;
    int ty = (int)area.y + 6;
    DrawText("dist[] y parent[]", tx, ty, 13, Pal::TEXT_DIM());
    ty += 18;

    // Header
    int col0 = tx, col1 = tx + 60, col2 = tx + 120;
    DrawText("Nodo", col0, ty, 11, Pal::CYAN_C());
    DrawText("dist", col1, ty, 11, Pal::CYAN_C());
    DrawText("parent", col2, ty, 11, Pal::CYAN_C());
    ty += 14;
    DrawLine((int)area.x + 4, ty, (int)(area.x + area.width) - 4, ty, Pal::BORDER());
    ty += 3;

    // Determine changed node in current step
    int changedNode = -1;
    if (stepIdx >= 0 && stepIdx < (int)steps.size()) {
        const AlgoStep& s = steps[stepIdx];
        if (s.type == StepType::BFS_ENQUEUE) changedNode = s.v;
    }

    int n = tree.n;
    for (int i = 0; i < n; i++) {
        if (ty + 13 > (int)(area.y + area.height)) break;

        bool highlight = (i == changedNode);
        if (highlight) {
            DrawRectangle((int)area.x + 2, ty - 1, (int)area.width - 4, 13,
                          {Pal::AMBER().r, Pal::AMBER().g, Pal::AMBER().b, 60});
        }

        Color rowC = highlight ? Pal::AMBER() : Pal::TEXT();
        std::string nstr = "v" + std::to_string(i);
        std::string dstr = "─";
        std::string pstr = "─";

        if (stepIdx >= 0 && stepIdx < (int)steps.size()) {
            const AlgoStep& s = steps[stepIdx];
            if (i < (int)s.distArr.size())   dstr = distStr(s.distArr[i]);
            if (i < (int)s.parentArr.size()) {
                int p = s.parentArr[i];
                pstr = (p == -1) ? "─" : ("v" + std::to_string(p));
            }
        }

        DrawText(nstr.c_str(), col0, ty, 11, rowC);
        DrawText(dstr.c_str(), col1, ty, 11, rowC);
        DrawText(pstr.c_str(), col2, ty, 11, rowC);
        ty += 14;
    }
}

// ════════════════════════════════════════════════════════════
//  drawSummaryCards()
// ════════════════════════════════════════════════════════════

void Visualizer::drawSummaryCards(Rectangle area) {
    int tx = (int)area.x + 8;
    int ty = (int)area.y + 6;
    DrawText("Resumen", tx, ty, 13, Pal::TEXT_DIM());
    ty += 18;

    // Phase
    std::string phase = "IDLE";
    if (stepIdx >= 0 && stepIdx < (int)steps.size()) {
        switch (steps[stepIdx].phase) {
            case BFSPhase::IDLE:        phase = "IDLE"; break;
            case BFSPhase::BFS1:        phase = "BFS 1"; break;
            case BFSPhase::BFS2:        phase = "BFS 2"; break;
            case BFSPhase::RECONSTRUCT: phase = "Reconstruyendo"; break;
            case BFSPhase::DONE:        phase = "¡Listo!"; break;
        }
    }

    std::string eA = "─", eB = "─", diam = "─";
    if (result.valid) {
        eA   = "v" + std::to_string(result.extremoA);
        eB   = "v" + std::to_string(result.extremoB);
        diam = std::to_string(result.diameter) + " arista(s)";
    }

    struct Card { const char* title; std::string val; Color col; };
    Card cards[] = {
        {"Fase",      phase, Pal::BLUE_C()},
        {"Extremo A", eA,    Pal::RED_C()},
        {"Extremo B", eB,    Pal::RED_C()},
        {"Diametro",  diam,  Pal::GREEN_C()}
    };

    int cw = (int)(area.width / 2) - 6;
    int ch = 28;
    int gap = 4;
    int cx  = tx - 4;
    int cy  = ty;
    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int rx  = (int)area.x + 4 + col * (cw + gap);
        int ry  = cy + row * (ch + gap);
        DrawRectangle(rx, ry, cw, ch, Pal::PANEL2());
        DrawRectangleLines(rx, ry, cw, ch, Pal::BORDER());
        DrawText(cards[i].title, rx + 4, ry + 2,  10, Pal::TEXT_DIM());
        int vw = MeasureText(cards[i].val.c_str(), 12);
        DrawText(cards[i].val.c_str(), rx + cw/2 - vw/2, ry + ch/2 - 1, 12, cards[i].col);
    }
    (void)cx;
}

// ════════════════════════════════════════════════════════════
//  drawLegend()
// ════════════════════════════════════════════════════════════

void Visualizer::drawLegend(Rectangle area) {
    DrawRectangle((int)area.x, (int)area.y, (int)area.width, (int)area.height,
                  {18, 20, 32, 200});
    DrawRectangleLines((int)area.x, (int)area.y, (int)area.width, (int)area.height, Pal::BORDER());

    struct LegItem { Color col; const char* label; };
    LegItem items[] = {
        {Pal::UNVIS(),  "No visitado"},
        {Pal::AMBER(), "En la cola"},
        {Pal::CYAN_C(), "Nodo actual"},
        {Pal::STEEL(),  "Procesado"},
        {Pal::RED_C(),    "Extremo A / B"},
        {Pal::GREEN_C(),  "En el diametro"},
    };

    int tx = (int)area.x + 6;
    int ty = (int)area.y + 4;
    DrawText("Leyenda", tx, ty, 11, Pal::TEXT_DIM());
    ty += 14;

    for (auto& it : items) {
        DrawCircle(tx + 5, ty + 5, 5, it.col);
        DrawText(it.label, tx + 14, ty, 10, Pal::TEXT());
        ty += 14;
    }

    // Edge legend
    DrawLineEx({(float)(tx + 2), (float)(ty + 5)},
               {(float)(tx + 10), (float)(ty + 5)}, 3.0f, Pal::GREEN_C());
    DrawText("Camino diametro", tx + 14, ty, 10, Pal::TEXT());
}

// ════════════════════════════════════════════════════════════
//  drawTabIntro()  F1
// ════════════════════════════════════════════════════════════

void Visualizer::drawTabIntro() {
    int cx = W / 2;
    int y  = NAV_H + 24;

    // Title
    drawTextC("Diametro de un Arbol  -  O(n)", cx, y, 36, Pal::CYAN_C());
    y += 48;
    drawTextC("CS2023 Algoritmos y Estructuras de Datos  |  UTEC 2026-2", cx, y, 20, Pal::TEXT_DIM());
    y += 28;
    drawTextC("Prof. Victor Racso Galvan Oyola", cx, y, 18, Pal::TEXT_DIM());
    y += 36;

    // Members panel
    Rectangle membersPanel = {(float)(cx - 250), (float)y, 500.0f, 95.0f};
    drawPanel(membersPanel, Pal::PANEL2(), Pal::BORDER(), 8.0f);
    int my = y + 10;
    DrawText("Integrantes del Grupo:", cx - 240, my, 14, Pal::CYAN_C());
    my += 20;
    DrawText("  [INTEGRANTE_1]  -  [CODIGO_1]", cx - 240, my, 13, Pal::TEXT()); my += 18;
    DrawText("  [INTEGRANTE_2]  -  [CODIGO_2]", cx - 240, my, 13, Pal::TEXT()); my += 18;
    DrawText("  [INTEGRANTE_3]  -  [CODIGO_3]", cx - 240, my, 13, Pal::TEXT());
    y += 110;

    // Three cards
    struct InfoCard {
        const char* title;
        const char* body1;
        const char* body2;
        const char* body3;
        Color bg;
    };
    InfoCard cards[] = {
        {
            "Que es el diametro?",
            "El camino simple mas largo entre",
            "dos nodos del arbol. Se mide como",
            "num. de aristas o suma de pesos.",
            {20, 40, 80, 255}
        },
        {
            "Para que sirve?",
            "Analisis de redes. Minimizar la",
            "latencia maxima en topologias",
            "arbol. Programacion competitiva.",
            {20, 60, 40, 255}
        },
        {
            "Algoritmo O(n) - Doble BFS",
            "BFS 1: desde nodo X -> halla A.",
            "BFS 2: desde A -> halla nodo B.",
            "El camino A->B es el diametro.",
            {60, 50, 10, 255}
        }
    };

    int cw = 390, ch = 115, gap = 15;
    int totalW = 3 * cw + 2 * gap;
    int startX = cx - totalW / 2;

    for (int i = 0; i < 3; i++) {
        int rx = startX + i * (cw + gap);
        Rectangle cr = {(float)rx, (float)y, (float)cw, (float)ch};
        drawPanel(cr, cards[i].bg, Pal::BORDER(), 8.0f);
        DrawText(cards[i].title, rx + 10, y + 10, 15, Pal::CYAN_C());
        DrawLine(rx + 10, y + 29, rx + cw - 10, y + 29, Pal::BORDER());
        DrawText(cards[i].body1, rx + 10, y + 36, 13, Pal::TEXT());
        DrawText(cards[i].body2, rx + 10, y + 54, 13, Pal::TEXT());
        DrawText(cards[i].body3, rx + 10, y + 72, 13, Pal::TEXT());
    }

    y += ch + 20;
    drawTextC("Presiona F2 para abrir el Simulador  |  F3 para Casos Borde  |  F4 para Complejidad",
              cx, y, 14, Pal::TEXT_DIM());
}

// ════════════════════════════════════════════════════════════
//  drawTabSimulator() / drawTabEdgeCases()  – not separately drawn,
//  they share left/canvas/right/bottom methods
// ════════════════════════════════════════════════════════════

void Visualizer::drawTabSimulator() {
    // Handled via drawLeftPanel, drawCanvas, drawRightPanel, drawBottomBar
}

void Visualizer::drawTabEdgeCases() {
    // Handled via drawLeftPanel, drawCanvas, drawRightPanel, drawBottomBar
}

// ════════════════════════════════════════════════════════════
//  drawTabComplexity()  F4
// ════════════════════════════════════════════════════════════

void Visualizer::drawTabComplexity() {
    int cx = W / 2;
    int y  = NAV_H + 20;

    drawTextC("Analisis de Complejidad Temporal  -  Diametro de un Arbol",
              cx, y, 26, Pal::CYAN_C());
    y += 44;

    // ── Comparison bars ──────────────────────────────────────
    int barX   = 60;
    int barH   = 36;

    // Brute force
    DrawText("Fuerza Bruta O(n2):", barX, y, 14, Pal::TEXT_DIM());
    y += 18;
    DrawRectangle(barX, y, 580, barH, Pal::RED_C());
    DrawRectangleLines(barX, y, 580, barH, Pal::BORDER());
    int tw = MeasureText("BFS desde cada nodo: O(n) x n = O(n2)", 13);
    DrawText("BFS desde cada nodo: O(n) x n = O(n2)", barX + 290 - tw/2, y + barH/2 - 6, 13, Pal::TEXT());
    y += barH + 10;

    // Double BFS
    DrawText("Doble BFS O(n):", barX, y, 14, Pal::TEXT_DIM());
    y += 18;
    DrawRectangle(barX, y, 160, barH, Pal::GREEN_C());
    DrawRectangleLines(barX, y, 160, barH, Pal::BORDER());
    int tw2 = MeasureText("2xO(n) = O(n)", 13);
    DrawText("2xO(n) = O(n)", barX + 80 - tw2/2, y + barH/2 - 6, 13, Pal::DARK());
    y += barH + 24;

    // ── Breakdown table ───────────────────────────────────────
    DrawLine(barX, y, W - 60, y, Pal::BORDER());
    y += 8;
    DrawText("Desglose de Nuestro Algoritmo:", barX, y, 15, Pal::CYAN_C());
    y += 22;

    struct Row { const char* step; const char* cost; };
    Row rows[] = {
        {"BFS 1: visita n nodos + n-1 aristas",    "O(n)"},
        {"BFS 2: igual que BFS 1",                 "O(n)"},
        {"Reconstruccion del camino: <= n pasos",  "O(n)"},
        {"Total:",                                  "O(n) + O(n) + O(n) = O(n)"},
        {"Espacio: adj + dist[] + parent[] + queue","O(n)"}
    };

    int col0 = barX, col1 = barX + 460;
    for (auto& r : rows) {
        DrawText(r.step, col0, y, 13, Pal::TEXT());
        DrawText(r.cost, col1, y, 13, Pal::GREEN_C());
        y += 18;
    }
    y += 10;

    // ── Credits ───────────────────────────────────────────────
    DrawLine(barX, y, W - 60, y, Pal::BORDER());
    y += 10;

    Rectangle credBox = {(float)(cx - 350), (float)y, 700.0f, 120.0f};
    drawPanel(credBox, Pal::PANEL2(), Pal::BORDER(), 8.0f);

    int cy2 = y + 10;
    drawTextC("Proyecto: Diametro de un Arbol en O(n) | CS2023 AED | UTEC 2026-2",
              cx, cy2, 13, Pal::CYAN_C());
    cy2 += 22;
    DrawText("  [INTEGRANTE_1]  -  [CODIGO_1]", cx - 330, cy2, 12, Pal::TEXT()); cy2 += 18;
    DrawText("  [INTEGRANTE_2]  -  [CODIGO_2]", cx - 330, cy2, 12, Pal::TEXT()); cy2 += 18;
    DrawText("  [INTEGRANTE_3]  -  [CODIGO_3]", cx - 330, cy2, 12, Pal::TEXT()); cy2 += 18;
    DrawText("GitHub: https://github.com/Eddddddd06/Proyecto2_AED_Diametro_de_arbol",
             cx - 330, cy2, 12, Pal::TEXT_DIM());
}
