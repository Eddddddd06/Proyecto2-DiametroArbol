#include "Visualizer.h"
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

Viz::Viz() {}

Viz::~Viz() {
    if (fCarg) {
        if (fReg.texture.id > 1) UnloadFont(fReg);
        if (fBold.texture.id > 1) UnloadFont(fBold);
        if (fMono.texture.id > 1) UnloadFont(fMono);
    }
}

Font Viz::intentarFuente(int size) {
    const char* rutas[] = {
        "/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/Hack-Regular.ttf",
        "/usr/share/fonts/nerd-fonts-complete/TTF/JetBrainsMono-Regular.ttf"
    };
    for (auto& p : rutas) {
        if (FileExists(p)) {
            Font f = LoadFontEx(p, size, nullptr, 512);
            if (f.texture.id > 0) return f;
        }
    }
    return GetFontDefault();
}

void Viz::cargarFuentes() {
    fReg = intentarFuente(18);
    fBold = intentarFuente(20);
    fMono = intentarFuente(16);
    fCarg = true;
}

void Viz::init() {
    cargarFuentes();

    bTabs[0].rect = {5, 2, 310, 46};
    bTabs[0].txt = "F1  Portada & Intro";
    bTabs[0].atajo = "";
    bTabs[0].alClick = [this]{ tabAct = Tab::INTRO; };

    bTabs[1].rect = {320, 2, 310, 46};
    bTabs[1].txt = "F2  Simulador Principal";
    bTabs[1].atajo = "";
    bTabs[1].alClick = [this]{ tabAct = Tab::SIMULATOR; };

    bTabs[2].rect = {635, 2, 310, 46};
    bTabs[2].txt = "F3  Casos Borde";
    bTabs[2].atajo = "";
    bTabs[2].alClick = [this]{ tabAct = Tab::EDGE_CASES; };

    bTabs[3].rect = {950, 2, 320, 46};
    bTabs[3].txt = "F4  Complejidad & Créditos";
    bTabs[3].atajo = "";
    bTabs[3].alClick = [this]{ tabAct = Tab::COMPLEXITY; };

    bPlay.rect = {5, 70, 220, 38};
    bPlay.txt = "▶  Play / ⏸ Pausa";
    bPlay.atajo = "[Espacio]";
    bPlay.alClick = [this]{
        reprod = !reprod;
        if (reprod && iPaso == -1) avanzar();
    };

    bSig.rect = {5, 114, 107, 34};
    bSig.txt = "⏭ Siguiente";
    bSig.atajo = "[→]";
    bSig.alClick = [this]{ avanzar(); };

    bAnt.rect = {118, 114, 107, 34};
    bAnt.txt = "⏮ Anterior";
    bAnt.atajo = "[←]";
    bAnt.alClick = [this]{ retroceder(); };

    bReset.rect = {5, 154, 220, 34};
    bReset.txt = "🔄 Reiniciar";
    bReset.atajo = "[R]";
    bReset.alClick = [this]{ reiniciar(); };

    bVelMen.rect = {5, 194, 105, 30};
    bVelMen.txt = "Velocidad −";
    bVelMen.atajo = "[-]";
    bVelMen.alClick = [this]{ vel = std::max(0.25f, vel - 0.25f); };

    bVelMas.rect = {116, 194, 109, 30};
    bVelMas.txt = "Velocidad +";
    bVelMas.atajo = "[+]";
    bVelMas.alClick = [this]{ vel = std::min(3.0f, vel + 0.25f); };

    bPeso.rect = {5, 230, 220, 30};
    bPeso.txt = "Alternar Árbol Ponderado";
    bPeso.atajo = "[W]";
    bPeso.alClick = [this]{
        modoPeso = !modoPeso;
        if (modoPeso) cargarArbol(TipoArbol::CON_PESO);
        else          cargarArbol(TipoArbol::SIN_PESO);
    };

    bCasos[0].rect = {5, 280, 220, 36};
    bCasos[0].txt = "Caso 1: Vacío (N=0)";
    bCasos[0].atajo = "";
    bCasos[0].alClick = [this]{
        cargarCaso(CasoBorde::VACIO);
        casoAct = CasoBorde::VACIO;
    };

    bCasos[1].rect = {5, 322, 220, 36};
    bCasos[1].txt = "Caso 2: Un Nodo (N=1)";
    bCasos[1].atajo = "";
    bCasos[1].alClick = [this]{
        cargarCaso(CasoBorde::UN_NODO);
        casoAct = CasoBorde::UN_NODO;
    };

    bCasos[2].rect = {5, 364, 220, 36};
    bCasos[2].txt = "Caso 3: Cadena N=6";
    bCasos[2].atajo = "";
    bCasos[2].alClick = [this]{
        cargarCaso(CasoBorde::LINEAL);
        casoAct = CasoBorde::LINEAL;
    };

    bCasos[3].rect = {5, 406, 220, 36};
    bCasos[3].txt = "Caso 4: Estrella N=7";
    bCasos[3].atajo = "";
    bCasos[3].alClick = [this]{
        cargarCaso(CasoBorde::ESTRELLA);
        casoAct = CasoBorde::ESTRELLA;
    };

    cargarArbol(TipoArbol::SIN_PESO);
}


void Viz::cargarArbol(TipoArbol inst) {
    tree.resize(10);
    if (inst == TipoArbol::SIN_PESO) {
        tree.agregarAri(0, 1, 1); tree.agregarAri(0, 2, 1); tree.agregarAri(0, 3, 1);
        tree.agregarAri(1, 4, 1); tree.agregarAri(1, 5, 1);
        tree.agregarAri(2, 6, 1);
        tree.agregarAri(3, 7, 1);
        tree.agregarAri(4, 8, 1);
        tree.agregarAri(7, 9, 1);
    } else {
        tree.agregarAri(0, 1, 3); tree.agregarAri(0, 2, 1); tree.agregarAri(0, 3, 4);
        tree.agregarAri(1, 4, 2); tree.agregarAri(1, 5, 5);
        tree.agregarAri(2, 6, 1);
        tree.agregarAri(3, 7, 3);
        tree.agregarAri(4, 8, 6);
        tree.agregarAri(7, 9, 2);
    }
    calcLayout();
    initVis();
    reiniciar();
}

void Viz::cargarCaso(CasoBorde ec) {
    switch (ec) {
        case CasoBorde::VACIO:
            tree.clear();
            visNodos.clear();
            visAris.clear();
            reiniciar();
            break;
        case CasoBorde::UN_NODO:
            tree.resize(1);
            calcLayout();
            initVis();
            reiniciar();
            break;
        case CasoBorde::LINEAL:
            tree.resize(6);
            tree.agregarAri(0, 1, 1); tree.agregarAri(1, 2, 1); tree.agregarAri(2, 3, 1);
            tree.agregarAri(3, 4, 1); tree.agregarAri(4, 5, 1);
            calcLayout();
            initVis();
            reiniciar();
            break;
        case CasoBorde::ESTRELLA:
            tree.resize(7);
            tree.agregarAri(0, 1, 1); tree.agregarAri(0, 2, 1); tree.agregarAri(0, 3, 1);
            tree.agregarAri(0, 4, 1); tree.agregarAri(0, 5, 1); tree.agregarAri(0, 6, 1);
            calcLayout();
            initVis();
            reiniciar();
            break;
    }
}


void Viz::calcLayout() {
    int n = tree.n;
    posNodos.clear();
    posNodos.resize(n);

    if (n == 0) return;
    if (n == 1) {
        posNodos[0] = {390.0f, 280.0f};
        return;
    }

    std::vector<int> depth(n, -1);
    std::vector<std::vector<int>> porNivel;
    std::queue<int> bfsQ;
    depth[0] = 0;
    bfsQ.push(0);
    porNivel.push_back({0});

    while (!bfsQ.empty()) {
        int u = bfsQ.front(); bfsQ.pop();
        for (auto& e : tree.adj[u]) {
            int v = e.dest;
            if (depth[v] == -1) {
                depth[v] = depth[u] + 1;
                bfsQ.push(v);
                while ((int)porNivel.size() <= depth[v])
                    porNivel.push_back({});
                porNivel[depth[v]].push_back(v);
            }
        }
    }

    int maxProf = (int)porNivel.size() - 1;

    bool esLineal = (maxProf >= n - 1);
    if (!esLineal) {
        esLineal = true;
        for (auto& lvl : porNivel) {
            if ((int)lvl.size() != 1) { esLineal = false; break; }
        }
    }

    bool esEstrella = false;
    if (n > 1 && maxProf == 1) {
        esEstrella = true;
        for (auto& e : tree.adj[0]) {
            int v = e.dest;
            if ((int)tree.adj[v].size() != 1) { esEstrella = false; break; }
        }
    }

    const float CW = (float)CANVAS_W;
    const float CH = (float)CANVAS_H;

    if (esLineal) {
        float padding = 60.0f;
        float step = (n > 1) ? (CW - 2.0f * padding) / (float)(n - 1) : 0.0f;
        float y = CH / 2.0f;
        std::vector<int> orden;
        {
            std::queue<int> q2;
            std::vector<bool> vis(n, false);
            q2.push(0); vis[0] = true;
            while (!q2.empty()) {
                int u = q2.front(); q2.pop();
                orden.push_back(u);
                for (auto& e : tree.adj[u]) {
                    if (!vis[e.dest]) { vis[e.dest] = true; q2.push(e.dest); }
                }
            }
        }
        for (int i = 0; i < n; ++i) {
            posNodos[orden[i]] = {padding + i * step, y};
        }
    } else if (esEstrella) {
        posNodos[0] = {390.0f, 280.0f};
        int hojas = (int)tree.adj[0].size();
        float radio = 200.0f;
        for (int i = 0; i < hojas; i++) {
            float ang = -M_PI / 2.0f + 2.0f * M_PI * i / (float)hojas;
            posNodos[tree.adj[0][i].dest] = {
                390.0f + radio * std::cos(ang),
                280.0f + radio * std::sin(ang)
            };
        }
    } else {
        float topPad = 70.0f;
        float botPad = 50.0f;
        float leftPad = 70.0f;
        float rightPad = 70.0f;
        float nivelH = (CH - topPad - botPad) / (float)std::max(1, maxProf);

        for (int d = 0; d <= maxProf; ++d) {
            auto& lvl = porNivel[d];
            int cnt = (int)lvl.size();
            float y = topPad + d * nivelH;
            if (cnt == 1) {
                posNodos[lvl[0]] = {CW / 2.0f, y};
            } else {
                float step = (CW - leftPad - rightPad) / (float)(cnt - 1);
                for (int i = 0; i < cnt; i++) {
                    posNodos[lvl[i]] = {leftPad + i * step, y};
                }
            }
        }
    }
}

void Viz::initVis() {
    int n = tree.n;
    visNodos.resize(n);
    for (int i = 0; i < n; ++i) {
        visNodos[i].pos = (n > 0 && i < (int)posNodos.size())
                          ? posNodos[i] : Vector2{390, 280};
        visNodos[i].col = Pal::UNVIS();
        visNodos[i].colObj = Pal::UNVIS();
        visNodos[i].esc = 1.0f;
        visNodos[i].escObj = 1.0f;
    }
    int E = (int)tree.listaAri.size();
    visAris.resize(E);
    for (int i = 0; i < E; i++) {
        visAris[i].col = Pal::BORDER();
        visAris[i].colObj = Pal::BORDER();
        visAris[i].prog = 1.0f;
        visAris[i].progObj = 1.0f;
    }
}

void Viz::reiniciar() {
    iPaso = -1;
    reprod = false;
    timerRep = 0.0f;
    pasos.clear();
    res = Tree::Result{};
    txtDesc = "Presiona [Espacio] o [ ▶ Play ] para iniciar la simulación.";
    colaVis.clear();
    initVis();
}


void Viz::irAPaso(int idx) {
    if (pasos.empty() || idx < 0 || idx >= (int)pasos.size()) return;
    iPaso = idx;
    const Paso& s = pasos[idx];
    int n = tree.n;
    for (int i = 0; i < n; i++) {
        visNodos[i].colObj = colorNodo(s.estNodos[i]);
        if (s.estNodos[i] == EstNodo::ACTUAL || s.estNodos[i] == EstNodo::EN_DIAM)
            visNodos[i].escObj = 1.15f;
        else
            visNodos[i].escObj = 1.0f;
    }
    int E = (int)tree.listaAri.size();
    for (int i = 0; i < E; ++i) {
        visAris[i].colObj = colorAri(s.estAris[i]);
    }
    colaVis = s.estCola;
    txtDesc = s.desc;
}

void Viz::avanzar() {
    if (pasos.empty()) {
        pasos = tree.calcPasos(nodoIni);
        res = Tree::sacarRes(pasos);
    }
    if (iPaso + 1 < (int)pasos.size()) {
        irAPaso(iPaso + 1);
    } else {
        reprod = false;
    }
}

void Viz::retroceder() {
    if (iPaso > 0) {
        irAPaso(iPaso - 1);
    } else if (iPaso == 0) {
        reiniciar();
    }
}


int Viz::nodoEnMouse(Vector2 mp) const {
    int n = tree.n;
    for (int i = 0; i < n; i++) {
        Vector2 wp = {visNodos[i].pos.x + (float)LEFT_W,
                      visNodos[i].pos.y + (float)NAV_H};
        if (enCirc(wp, (float)(NR + 4), mp)) return i;
    }
    return -1;
}

void Viz::teclasTabs() {
    if (IsKeyPressed(KEY_F1)) tabAct = Tab::INTRO;
    if (IsKeyPressed(KEY_F2)) tabAct = Tab::SIMULATOR;
    if (IsKeyPressed(KEY_F3)) tabAct = Tab::EDGE_CASES;
    if (IsKeyPressed(KEY_F4)) tabAct = Tab::COMPLEXITY;
}

void Viz::teclasReprod() {
    if (IsKeyPressed(KEY_SPACE))  bPlay.alClick();
    if (IsKeyPressed(KEY_RIGHT))  avanzar();
    if (IsKeyPressed(KEY_LEFT))   retroceder();
    if (IsKeyPressed(KEY_R))      reiniciar();
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        vel = std::max(0.25f, vel - 0.25f);
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        vel = std::min(3.0f, vel + 0.25f);
    if (IsKeyPressed(KEY_W)) bPeso.alClick();
}

void Viz::manejarInput() {
    Vector2 mp = GetMousePosition();
    bool click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    for (auto& btn : bTabs) {
        if (click && enRect(btn.rect, mp)) { btn.alClick(); return; }
    }

    Boton* ctrl[] = {
        &bPlay, &bSig, &bAnt, &bReset,
        &bVelMen, &bVelMas, &bPeso
    };
    for (auto* btn : ctrl) {
        if (click && enRect(btn->rect, mp)) { btn->alClick(); return; }
    }

    if (tabAct == Tab::EDGE_CASES) {
        for (auto& btn : bCasos) {
            if (click && enRect(btn.rect, mp)) { btn.alClick(); return; }
        }
    }

    if (tabAct == Tab::SIMULATOR || tabAct == Tab::EDGE_CASES) {
        if (click) {
            int idx = nodoEnMouse(mp);
            if (idx >= 0) {
                nodoIni = idx;
                reiniciar();
                return;
            }
        }
    }

    teclasTabs();
    teclasReprod();
}


void Viz::update() {
    float dt = GetFrameTime();
    tAnim += dt;

    if (reprod) {
        timerRep += dt;
        if (timerRep >= intPaso / vel) {
            avanzar();
            timerRep = 0.0f;
        }
    }

    for (auto& nv : visNodos) {
        nv.col = lerpC(nv.col, nv.colObj, 0.12f);
        nv.esc = nv.esc + (nv.escObj - nv.esc) * 0.12f;
    }
    for (auto& ev : visAris) {
        ev.col = lerpC(ev.col, ev.colObj, 0.12f);
    }

    nodHov = nodoEnMouse(GetMousePosition());

    manejarInput();
}

bool Viz::shouldClose() const {
    return WindowShouldClose();
}


void Viz::dibPanel(Rectangle r, Color fill, Color border, float rad) {
    float ratio = rad / std::max(r.width, r.height);
    DrawRectangleRounded(r, ratio, 6, fill);
    if (border.a > 0) {
        DrawRectangleRoundedLines(r, ratio, 6, border);
    }
}

void Viz::textoC(const char* txt, int cx, int cy, int sz, Color c) {
    int w = MeasureText(txt, sz);
    DrawText(txt, cx - w / 2, cy - sz / 2, sz, c);
}

void Viz::textoEx(const char* txt, float x, float y, float sz, float sp, Color c, Font f) {
    DrawTextEx(f, txt, {x, y}, sz, sp, c);
}

void Viz::etiqueta(const char* txt, int x, int y, int sz, Color c) {
    DrawText(txt, x, y, sz, c);
}

void Viz::dibBoton(Boton& btn) {
    btn.hover = enRect(btn.rect, GetMousePosition());
    Color bg = btn.activo  ? Pal::STEEL()  :
               btn.hover ? Pal::BORDER() : Pal::PANEL2();
    DrawRectangleRounded(btn.rect, 0.3f, 6, bg);
    DrawRectangleRoundedLines(btn.rect, 0.3f, 6, Pal::BORDER());

    int bx = (int)(btn.rect.x + btn.rect.width / 2);
    int by = (int)(btn.rect.y + btn.rect.height / 2);

    if (!btn.atajo.empty()) {
        int lsz = 13;
        int ssz = 10;
        int lw = MeasureText(btn.txt.c_str(), lsz);
        int sw = MeasureText(btn.atajo.c_str(), ssz);
        DrawText(btn.txt.c_str(), bx - lw / 2, by - lsz, lsz, Pal::TEXT());
        DrawText(btn.atajo.c_str(), bx - sw / 2, by + 1, ssz, Pal::TEXT_DIM());
    } else {
        int lsz = 13;
        int lw = MeasureText(btn.txt.c_str(), lsz);
        DrawText(btn.txt.c_str(), bx - lw / 2, by - lsz / 2, lsz, Pal::TEXT());
    }
}


Color Viz::colorNodo(EstNodo s) const {
    switch (s) {
        case EstNodo::NO_VIS:    return Pal::UNVIS();
        case EstNodo::EN_COLA:   return Pal::AMBER();
        case EstNodo::ACTUAL:    return Pal::CYAN_C();
        case EstNodo::PROCESADO: return Pal::STEEL();
        case EstNodo::EXTREMO_A: return Pal::RED_C();
        case EstNodo::EXTREMO_B: return Pal::RED_C();
        case EstNodo::EN_DIAM:   return Pal::GREEN_C();
        default:                 return Pal::UNVIS();
    }
}

Color Viz::colorAri(EstAri s) const {
    switch (s) {
        case EstAri::NO_VIS:  return {40, 45, 65, 255};
        case EstAri::ARI_BFS: return {94, 129, 172, 200};
        case EstAri::EN_DIAM: return Pal::GREEN_C();
        default:              return {40, 45, 65, 255};
    }
}

Color Viz::lerpC(Color a, Color b, float t) const {
    t = std::clamp(t, 0.0f, 1.0f);
    return Color{
        (unsigned char)(a.r + (b.r - a.r) * t),
        (unsigned char)(a.g + (b.g - a.g) * t),
        (unsigned char)(a.b + (b.b - a.b) * t),
        (unsigned char)(a.a + (b.a - a.a) * t)
    };
}

float Viz::easeIO(float t) const {
    return 3.0f * t * t - 2.0f * t * t * t;
}

std::string Viz::strDst(int d) const {
    if (d == -1) return "\xe2\x88\x9e";
    return std::to_string(d);
}

bool Viz::enRect(Rectangle r, Vector2 p) const {
    return CheckCollisionPointRec(p, r);
}

bool Viz::enCirc(Vector2 center, float r, Vector2 p) const {
    return Vector2Distance(p, center) <= r;
}


void Viz::draw() {
    ClearBackground(Pal::BG());
    dibujarNav();

    switch (tabAct) {
        case Tab::INTRO:
            dibTabIntro();
            break;
        case Tab::SIMULATOR:
            dibujarIzq();
            dibujarCanvas();
            dibujarDer();
            dibujarAbajo();
            break;
        case Tab::EDGE_CASES:
            dibujarIzq();
            dibujarCanvas();
            dibujarDer();
            dibujarAbajo();
            break;
        case Tab::COMPLEXITY:
            dibTabComplejidad();
            break;
    }
}

void Viz::dibujarNav() {
    DrawRectangle(0, 0, W, NAV_H, Pal::PANEL());
    DrawLine(0, NAV_H, W, NAV_H, Pal::BORDER());

    for (int i = 0; i < 4; ++i) {
        bool act = ((int)tabAct == i);
        bTabs[i].activo = act;
        dibBoton(bTabs[i]);
    }
}

void Viz::dibujarIzq() {
    DrawRectangle(0, NAV_H, LEFT_W, H - NAV_H, Pal::PANEL());
    DrawLine(LEFT_W, NAV_H, LEFT_W, H, Pal::BORDER());

    etiqueta("Controles", 10, NAV_H + 10, 14, Pal::TEXT_DIM());

    bPlay.activo = reprod;
    dibBoton(bPlay);
    dibBoton(bSig);
    dibBoton(bAnt);
    dibBoton(bReset);
    dibBoton(bVelMen);
    dibBoton(bVelMas);
    bPeso.activo = modoPeso;
    dibBoton(bPeso);

    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << "Velocidad: " << vel << "x";
        etiqueta(ss.str().c_str(), 10, 232, 11, Pal::AMBER());
    }

    if (tabAct == Tab::EDGE_CASES) {
        DrawLine(5, 263, LEFT_W - 5, 263, Pal::BORDER());
        etiqueta("Casos Borde:", 10, 267, 13, Pal::TEXT_DIM());
        for (int i = 0; i < 4; i++) {
            bCasos[i].activo = (casoAct == (CasoBorde)i);
            dibBoton(bCasos[i]);
        }
    }

    {
        DrawLine(5, H - NAV_H - 115, LEFT_W - 5, H - NAV_H - 115, Pal::BORDER());
        etiqueta("Nodo Inicial:", 10, H - NAV_H - 110, 13, Pal::TEXT_DIM());
        std::string ini = "Inicio: v" + std::to_string(nodoIni);
        etiqueta(ini.c_str(), 10, H - NAV_H - 90, 15, Pal::CYAN_C());
        etiqueta("Clic en nodo", 10, H - NAV_H - 68, 11, Pal::TEXT_DIM());
        etiqueta("para cambiar", 10, H - NAV_H - 55, 11, Pal::TEXT_DIM());
    }
}


void Viz::dibujarCanvas() {
    Rectangle canvasRect = {(float)LEFT_W, (float)NAV_H, (float)CANVAS_W, (float)CANVAS_H};
    dibPanel(canvasRect, Pal::PANEL2(), Pal::BORDER(), 4.0f);

    BeginScissorMode(LEFT_W, NAV_H, CANVAS_W, CANVAS_H);

    if (tree.n == 0) {
        textoC("Árbol Vacío (N = 0)", LEFT_W + CANVAS_W / 2, NAV_H + CANVAS_H / 2, 22, Pal::TEXT_DIM());
    } else {
        dibujarArbol();
    }

    dibLeyenda({(float)(LEFT_W + CANVAS_W - 170), (float)(NAV_H + 5), 165.0f, 130.0f});

    EndScissorMode();
}

void Viz::dibujarDer() {
    DrawRectangle(RIGHT_X, NAV_H, RIGHT_W, H - NAV_H, Pal::PANEL());
    DrawLine(RIGHT_X, NAV_H, RIGHT_X, H, Pal::BORDER());

    int panH = H - NAV_H;
    int topH = (int)(panH * 0.40f);
    int midH = (int)(panH * 0.35f);
    int botH = panH - topH - midH;

    Rectangle topArea = {(float)RIGHT_X, (float)NAV_H, (float)RIGHT_W, (float)topH};
    Rectangle midArea = {(float)RIGHT_X, (float)(NAV_H+topH), (float)RIGHT_W, (float)midH};
    Rectangle botArea = {(float)RIGHT_X, (float)(NAV_H+topH+midH), (float)RIGHT_W, (float)botH};

    dibPanelCola(topArea);
    dibTablaDst(midArea);
    dibResumen(botArea);
}

void Viz::dibujarAbajo() {
    DrawRectangle(LEFT_W, BOT_Y, CANVAS_W, BOT_H, {18, 20, 32, 255});
    DrawLine(LEFT_W, BOT_Y, LEFT_W + CANVAS_W, BOT_Y, Pal::BORDER());

    std::string text = txtDesc;
    const int maxChars = 95;
    int x = LEFT_W + 10;
    int y = BOT_Y + 10;
    int lineH = 17;

    while (!text.empty()) {
        if ((int)text.size() <= maxChars) {
            DrawText(text.c_str(), x, y, 15, Pal::TEXT());
            break;
        }
        size_t cut = maxChars;
        size_t sp = text.rfind(' ', cut);
        if (sp == std::string::npos) sp = cut;
        DrawText(text.substr(0, sp).c_str(), x, y, 15, Pal::TEXT());
        text = text.substr(sp + 1);
        y += lineH;
        if (y > BOT_Y + BOT_H - lineH) break;
    }
}

void Viz::dibujarArbol() {
    int E = (int)tree.listaAri.size();
    for (int i = 0; i < E; i++)
        dibArista(i);
    for (int i = 0; i < tree.n; ++i)
        dibNodo(i);
}

void Viz::dibArista(int eIdx) {
    auto [u, v] = tree.listaAri[eIdx];
    if (u >= tree.n || v >= tree.n) return;

    Vector2 p1 = visNodos[u].pos;
    Vector2 p2 = visNodos[v].pos;
    Color c = visAris[eIdx].col;

    float thickness = 2.5f;
    bool enDiam = (visAris[eIdx].colObj.r == Pal::GREEN_C().r &&
                   visAris[eIdx].colObj.g == Pal::GREEN_C().g &&
                   visAris[eIdx].colObj.b == Pal::GREEN_C().b);
    if (enDiam) thickness = 4.5f;

    Vector2 wp1 = {p1.x + (float)LEFT_W, p1.y + (float)NAV_H};
    Vector2 wp2 = {p2.x + (float)LEFT_W, p2.y + (float)NAV_H};

    if (enDiam) {
        Color glow = {Pal::GREEN_C().r, Pal::GREEN_C().g, Pal::GREEN_C().b, 60};
        DrawLineEx(wp1, wp2, thickness + 6.0f, glow);
    }

    DrawLineEx(wp1, wp2, thickness, c);

    if (modoPeso && eIdx < (int)tree.pesosAri.size()) {
        int w = tree.pesosAri[eIdx];
        Vector2 mid = {(wp1.x + wp2.x) / 2.0f, (wp1.y + wp2.y) / 2.0f};
        std::string ws = std::to_string(w);
        int tsz = 12;
        int tw = MeasureText(ws.c_str(), tsz);
        DrawRectangle((int)mid.x - tw/2 - 2, (int)mid.y - tsz/2 - 1, tw + 4, tsz + 2, {18, 20, 32, 200});
        DrawText(ws.c_str(), (int)mid.x - tw/2, (int)mid.y - tsz/2, tsz, Pal::TEXT_DIM());
    }
}

void Viz::dibNodo(int idx) {
    if (idx >= (int)visNodos.size()) return;
    Vector2 wp = {visNodos[idx].pos.x + (float)LEFT_W,
                  visNodos[idx].pos.y + (float)NAV_H};
    Color c = visNodos[idx].col;
    float s = visNodos[idx].esc;
    int r = (int)(NR * s);

    if (idx == nodHov) {
        DrawCircle((int)wp.x, (int)wp.y, r + 6, {Pal::TEXT().r, Pal::TEXT().g, Pal::TEXT().b, 40});
    }

    // reset manual por bug visual
    if (idx == nodoIni && iPaso == -1) {
        DrawCircle((int)wp.x, (int)wp.y, r + 8,
                   {Pal::AMBER().r, Pal::AMBER().g, Pal::AMBER().b, 60});
    }

    if (iPaso >= 0 && iPaso < (int)pasos.size()) {
        EstNodo ns = pasos[iPaso].estNodos[idx];
        if (ns == EstNodo::ACTUAL || ns == EstNodo::EN_DIAM) {
            float pulse = 0.5f + 0.5f * std::sin(tAnim * 5.0f);
            DrawCircle((int)wp.x, (int)wp.y, r + 4 + (int)(pulse * 3),
                       {c.r, c.g, c.b, 50});
        }
    }

    DrawCircle((int)wp.x, (int)wp.y, r, c);
    DrawCircleLines((int)wp.x, (int)wp.y, (float)r, {Pal::TEXT().r, Pal::TEXT().g, Pal::TEXT().b, 180});

    std::string lbl = "v" + std::to_string(idx);
    int bright = (int)c.r + c.g + c.b;
    Color textCol = (bright > 420) ? Pal::DARK() : Pal::TEXT();
    int tsz = 11;
    int tw = MeasureText(lbl.c_str(), tsz);
    DrawText(lbl.c_str(), (int)wp.x - tw/2, (int)wp.y - tsz/2, tsz, textCol);

    if (iPaso >= 0 && iPaso < (int)pasos.size() &&
        idx < (int)pasos[iPaso].arrDst.size()) {
        int d = pasos[iPaso].arrDst[idx];
        std::string ds = "d=" + strDst(d);
        int dsz = 10;
        int dw = MeasureText(ds.c_str(), dsz);
        int px = (int)wp.x - dw/2 - 2;
        int py = (int)wp.y + r + 2;
        DrawRectangle(px, py, dw + 4, dsz + 2, {30, 34, 50, 200});
        DrawText(ds.c_str(), px + 2, py + 1, dsz, Pal::TEXT_DIM());
    }
}


void Viz::dibPanelCola(Rectangle area) {
    DrawLine((int)area.x, (int)(area.y + area.height),
             (int)(area.x + area.width), (int)(area.y + area.height), Pal::BORDER());

    int tx = (int)area.x + 8;
    int ty = (int)area.y + 6;
    DrawText("Cola BFS", tx, ty, 13, Pal::TEXT_DIM());
    ty += 20;

    if (iPaso < 0) {
        DrawText("[ esperando ]", tx, ty, 12, Pal::TEXT_DIM());
        return;
    }

    if (colaVis.empty()) {
        DrawText("[ Vacia ]", tx, ty, 12, Pal::TEXT_DIM());
        return;
    }

    int bw = 32, bh = 22, gap = 4;
    int x = tx;
    for (int node : colaVis) {
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

void Viz::dibTablaDst(Rectangle area) {
    DrawLine((int)area.x, (int)(area.y + area.height),
             (int)(area.x + area.width), (int)(area.y + area.height), Pal::BORDER());

    int tx = (int)area.x + 8;
    int ty = (int)area.y + 6;
    DrawText("dist[] y parent[]", tx, ty, 13, Pal::TEXT_DIM());
    ty += 18;

    int col0 = tx, col1 = tx + 60, col2 = tx + 120;
    DrawText("Nodo", col0, ty, 11, Pal::CYAN_C());
    DrawText("dist", col1, ty, 11, Pal::CYAN_C());
    DrawText("parent", col2, ty, 11, Pal::CYAN_C());
    ty += 14;
    DrawLine((int)area.x + 4, ty, (int)(area.x + area.width) - 4, ty, Pal::BORDER());
    ty += 3;

    int cambiado = -1;
    if (iPaso >= 0 && iPaso < (int)pasos.size()) {
        const Paso& s = pasos[iPaso];
        if (s.tipo == TipoPaso::BFS_METE) cambiado = s.v;
    }

    int n = tree.n;
    for (int i = 0; i < n; ++i) {
        if (ty + 13 > (int)(area.y + area.height)) break;

        bool hl = (i == cambiado);
        if (hl) {
            DrawRectangle((int)area.x + 2, ty - 1, (int)area.width - 4, 13,
                          {Pal::AMBER().r, Pal::AMBER().g, Pal::AMBER().b, 60});
        }

        Color rowC = hl ? Pal::AMBER() : Pal::TEXT();
        std::string nstr = "v" + std::to_string(i);
        std::string dstr = "─";
        std::string pstr = "─";

        if (iPaso >= 0 && iPaso < (int)pasos.size()) {
            const Paso& s = pasos[iPaso];
            if (i < (int)s.arrDst.size()) dstr = strDst(s.arrDst[i]);
            if (i < (int)s.arrPad.size()) {
                int p = s.arrPad[i];
                pstr = (p == -1) ? "─" : ("v" + std::to_string(p));
            }
        }

        DrawText(nstr.c_str(), col0, ty, 11, rowC);
        DrawText(dstr.c_str(), col1, ty, 11, rowC);
        DrawText(pstr.c_str(), col2, ty, 11, rowC);
        ty += 14;
    }
}

void Viz::dibResumen(Rectangle area) {
    int tx = (int)area.x + 8;
    int ty = (int)area.y + 6;
    DrawText("Resumen", tx, ty, 13, Pal::TEXT_DIM());
    ty += 18;

    std::string fase = "IDLE";
    if (iPaso >= 0 && iPaso < (int)pasos.size()) {
        switch (pasos[iPaso].fase) {
            case FaseBFS::IDLE:        fase = "IDLE"; break;
            case FaseBFS::BFS1:        fase = "BFS 1"; break;
            case FaseBFS::BFS2:        fase = "BFS 2"; break;
            case FaseBFS::RECONSTRUCT: fase = "Reconstruyendo"; break;
            case FaseBFS::DONE:        fase = "¡Listo!"; break;
        }
    }

    std::string eA = "─", eB = "─", diam = "─";
    if (res.ok) {
        eA = "v" + std::to_string(res.extremoA);
        eB = "v" + std::to_string(res.extremoB);
        diam = std::to_string(res.diam) + " arista(s)";
    }

    struct Card { const char* title; std::string val; Color col; };
    Card cards[] = {
        {"Fase",      fase, Pal::BLUE_C()},
        {"Extremo A", eA,   Pal::RED_C()},
        {"Extremo B", eB,   Pal::RED_C()},
        {"Diametro",  diam, Pal::GREEN_C()}
    };

    int cw = (int)(area.width / 2) - 6;
    int ch = 28;
    int gap = 4;
    int cx = tx - 4;
    int cy = ty;
    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int rx = (int)area.x + 4 + col * (cw + gap);
        int ry = cy + row * (ch + gap);
        DrawRectangle(rx, ry, cw, ch, Pal::PANEL2());
        DrawRectangleLines(rx, ry, cw, ch, Pal::BORDER());
        DrawText(cards[i].title, rx + 4, ry + 2, 10, Pal::TEXT_DIM());
        int vw = MeasureText(cards[i].val.c_str(), 12);
        DrawText(cards[i].val.c_str(), rx + cw/2 - vw/2, ry + ch/2 - 1, 12, cards[i].col);
    }
    (void)cx;
}

void Viz::dibLeyenda(Rectangle area) {
    DrawRectangle((int)area.x, (int)area.y, (int)area.width, (int)area.height,
                  {18, 20, 32, 200});
    DrawRectangleLines((int)area.x, (int)area.y, (int)area.width, (int)area.height, Pal::BORDER());

    struct Item { Color col; const char* label; };
    Item items[] = {
        {Pal::UNVIS(),   "No visitado"},
        {Pal::AMBER(),   "En la cola"},
        {Pal::CYAN_C(),  "Nodo actual"},
        {Pal::STEEL(),   "Procesado"},
        {Pal::RED_C(),   "Extremo A / B"},
        {Pal::GREEN_C(), "En el diametro"},
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

    DrawLineEx({(float)(tx + 2), (float)(ty + 5)},
               {(float)(tx + 10), (float)(ty + 5)}, 3.0f, Pal::GREEN_C());
    DrawText("Camino diametro", tx + 14, ty, 10, Pal::TEXT());
}


void Viz::dibTabIntro() {
    int cx = W / 2;
    int y = NAV_H + 24;

    textoC("Diametro de un Arbol  -  O(n)", cx, y, 36, Pal::CYAN_C());
    y += 48;
    textoC("CS2023 Algoritmos y Estructuras de Datos  |  UTEC 2026-2", cx, y, 20, Pal::TEXT_DIM());
    y += 28;
    textoC("Prof. Victor Racso Galvan Oyola", cx, y, 18, Pal::TEXT_DIM());
    y += 36;

    Rectangle membersPanel = {(float)(cx - 250), (float)y, 500.0f, 95.0f};
    dibPanel(membersPanel, Pal::PANEL2(), Pal::BORDER(), 8.0f);
    int my = y + 10;
    DrawText("Integrantes del Grupo:", cx - 240, my, 14, Pal::CYAN_C());
    my += 20;
    DrawText("  Eduardo Osorio", cx - 240, my, 13, Pal::TEXT()); my += 18;
    DrawText("  Leonardo Sanchez Terrazos", cx - 240, my, 13, Pal::TEXT());
    y += 110;

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

    for (int i = 0; i < 3; ++i) {
        int rx = startX + i * (cw + gap);
        Rectangle cr = {(float)rx, (float)y, (float)cw, (float)ch};
        dibPanel(cr, cards[i].bg, Pal::BORDER(), 8.0f);
        DrawText(cards[i].title, rx + 10, y + 10, 15, Pal::CYAN_C());
        DrawLine(rx + 10, y + 29, rx + cw - 10, y + 29, Pal::BORDER());
        DrawText(cards[i].body1, rx + 10, y + 36, 13, Pal::TEXT());
        DrawText(cards[i].body2, rx + 10, y + 54, 13, Pal::TEXT());
        DrawText(cards[i].body3, rx + 10, y + 72, 13, Pal::TEXT());
    }

    y += ch + 20;
    textoC("Presiona F2 para abrir el Simulador  |  F3 para Casos Borde  |  F4 para Complejidad",
           cx, y, 14, Pal::TEXT_DIM());
}

void Viz::dibTabSim() {
}

void Viz::dibTabCasos() {
}

void Viz::dibTabComplejidad() {
    int cx = W / 2;
    int y = NAV_H + 20;

    textoC("Analisis de Complejidad Temporal  -  Diametro de un Arbol",
           cx, y, 26, Pal::CYAN_C());
    y += 44;

    int barX = 60;
    int barH = 36;

    DrawText("Fuerza Bruta O(n2):", barX, y, 14, Pal::TEXT_DIM());
    y += 18;
    DrawRectangle(barX, y, 580, barH, Pal::RED_C());
    DrawRectangleLines(barX, y, 580, barH, Pal::BORDER());
    int tw = MeasureText("BFS desde cada nodo: O(n) x n = O(n2)", 13);
    DrawText("BFS desde cada nodo: O(n) x n = O(n2)", barX + 290 - tw/2, y + barH/2 - 6, 13, Pal::TEXT());
    y += barH + 10;

    DrawText("Doble BFS O(n):", barX, y, 14, Pal::TEXT_DIM());
    y += 18;
    DrawRectangle(barX, y, 160, barH, Pal::GREEN_C());
    DrawRectangleLines(barX, y, 160, barH, Pal::BORDER());
    int tw2 = MeasureText("2xO(n) = O(n)", 13);
    DrawText("2xO(n) = O(n)", barX + 80 - tw2/2, y + barH/2 - 6, 13, Pal::DARK());
    y += barH + 24;

    DrawLine(barX, y, W - 60, y, Pal::BORDER());
    y += 8;
    DrawText("Desglose de Nuestro Algoritmo:", barX, y, 15, Pal::CYAN_C());
    y += 22;

    struct Row { const char* step; const char* cost; };
    Row rows[] = {
        {"BFS 1: visita n nodos + n-1 aristas",   "O(n)"},
        {"BFS 2: igual que BFS 1",                "O(n)"},
        {"Reconstruccion del camino: <= n pasos", "O(n)"},
        {"Total:",                                 "O(n) + O(n) + O(n) = O(n)"},
        {"Espacio: adj + dist[] + parent[] + queue","O(n)"}
    };

    int col0 = barX, col1 = barX + 460;
    for (auto& r : rows) {
        DrawText(r.step, col0, y, 13, Pal::TEXT());
        DrawText(r.cost, col1, y, 13, Pal::GREEN_C());
        y += 18;
    }
    y += 10;

    DrawLine(barX, y, W - 60, y, Pal::BORDER());
    y += 10;

    Rectangle credBox = {(float)(cx - 350), (float)y, 700.0f, 120.0f};
    dibPanel(credBox, Pal::PANEL2(), Pal::BORDER(), 8.0f);

    int cy2 = y + 10;
    textoC("Proyecto: Diametro de un Arbol en O(n) | CS2023 AED | UTEC 2026-2",
           cx, cy2, 13, Pal::CYAN_C());
    cy2 += 22;
    DrawText("  Eduardo Osorio", cx - 330, cy2, 12, Pal::TEXT()); cy2 += 18;
    DrawText("  Leonardo Sanchez Terrazos", cx - 330, cy2, 12, Pal::TEXT()); cy2 += 18;
    DrawText("GitHub: https://github.com/Eddddddd06/Proyecto2_AED_Diametro_de_arbol",
             cx - 330, cy2, 12, Pal::TEXT_DIM());
}
