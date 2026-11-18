#pragma once
#include "Tree.h"
#include <raylib.h>
#include <vector>
#include <string>
#include <functional>
#include <array>

enum class Tab { INTRO = 0, SIMULATOR = 1, EDGE_CASES = 2, COMPLEXITY = 3 };

enum class TipoArbol { SIN_PESO = 0, CON_PESO = 1 };

enum class CasoBorde { VACIO = 0, UN_NODO = 1, LINEAL = 2, ESTRELLA = 3 };

namespace Pal {
    inline Color BG()       { return {15, 17, 26, 255}; }
    inline Color PANEL()    { return {24, 27, 40, 255}; }
    inline Color PANEL2()   { return {30, 34, 50, 255}; }
    inline Color BORDER()   { return {59, 66, 82, 255}; }
    inline Color TEXT()     { return {236, 239, 244, 255}; }
    inline Color TEXT_DIM() { return {143, 153, 175, 255}; }
    inline Color CYAN_C()   { return {136, 192, 208, 255}; }
    inline Color BLUE_C()   { return {129, 161, 193, 255}; }
    inline Color AMBER()    { return {235, 203, 139, 255}; }
    inline Color GREEN_C()  { return {163, 190, 140, 255}; }
    inline Color RED_C()    { return {191, 97, 106, 255}; }
    inline Color STEEL()    { return {94, 129, 172, 255}; }
    inline Color ORANGE_C() { return {208, 135, 112, 255}; }
    inline Color UNVIS()    { return {59, 66, 82, 255}; }
    inline Color WHITE_C()  { return {236, 239, 244, 255}; }
    inline Color DARK()     { return {15, 17, 26, 255}; }
}

namespace Lay {
    constexpr int W = 1280;
    constexpr int H = 720;
    constexpr int NAV_H = 50;
    constexpr int LEFT_W = 230;
    constexpr int RIGHT_W = 270;
    constexpr int BOT_H = 110;
    constexpr int CANVAS_X = LEFT_W;
    constexpr int CANVAS_Y = NAV_H;
    constexpr int CANVAS_W = W - LEFT_W - RIGHT_W;
    constexpr int CANVAS_H = H - NAV_H - BOT_H;
    constexpr int RIGHT_X = W - RIGHT_W;
    constexpr int BOT_Y = H - BOT_H;
    constexpr int NR = 24;
}

struct Boton {
    Rectangle rect;
    std::string txt;
    std::string atajo;
    bool activo = false;
    bool hover = false;
    Color colBase = {24,27,40,255};
    Color colHov = {59,66,82,255};
    Color colAct = {94,129,172,255};
    std::function<void()> alClick;
};

struct NodVis {
    Vector2 pos = {0,0};
    Color col = {59,66,82,255};
    Color colObj = {59,66,82,255};
    float esc = 1.0f;
    float escObj = 1.0f;
};

struct AriVis {
    Color col = {59,66,82,255};
    Color colObj = {59,66,82,255};
    float prog = 1.0f;
    float progObj = 1.0f;
};

class Viz {
public:
    Viz();
    ~Viz();

    void init();
    void update();
    void draw();
    bool shouldClose() const;

private:
    Font fReg, fBold, fMono;
    bool fCarg = false;
    void cargarFuentes();
    Font intentarFuente(int size);

    Tab tabAct = Tab::INTRO;

    Tree tree;
    std::vector<Paso> pasos;
    int iPaso = -1;
    bool reprod = false;
    float timerRep = 0.0f;
    float vel = 1.0f;
    float intPaso = 0.75f;
    int nodoIni = 0;

    std::vector<NodVis> visNodos;
    std::vector<AriVis> visAris;
    std::vector<Vector2> posNodos;
    float tAnim = 0.0f;

    Tree::Result res;

    int nodHov = -1;
    bool modoPeso = false;

    std::vector<int> colaVis;

    CasoBorde casoAct = CasoBorde::VACIO;

    Boton bPlay, bSig, bAnt, bReset;
    Boton bVelMen, bVelMas, bPeso;
    std::array<Boton,4> bTabs;
    std::array<Boton,4> bCasos;

    std::string txtDesc = "Presiona [Espacio] o [ Play ] para iniciar la simulacion.";

    void cargarArbol(TipoArbol inst);
    void cargarCaso(CasoBorde ec);
    void calcLayout();
    void initVis();
    void reiniciar();

    void manejarInput();
    void teclasTabs();
    void teclasReprod();
    int nodoEnMouse(Vector2 mp) const;

    void irAPaso(int idx);
    void avanzar();
    void retroceder();

    void dibujarNav();
    void dibujarIzq();
    void dibujarCanvas();
    void dibujarDer();
    void dibujarAbajo();

    void dibTabIntro();
    void dibTabSim();
    void dibTabCasos();
    void dibTabComplejidad();

    void dibujarArbol();
    void dibNodo(int idx);
    void dibArista(int eIdx);
    void dibPanelCola(Rectangle area);
    void dibTablaDst(Rectangle area);
    void dibResumen(Rectangle area);
    void dibLeyenda(Rectangle area);

    void dibBoton(Boton& btn);
    void dibPanel(Rectangle r, Color fill, Color border, float rad = 8.0f);
    void textoC(const char* txt, int cx, int cy, int sz, Color c);
    void textoEx(const char* txt, float x, float y, float sz, float sp, Color c, Font f);
    void etiqueta(const char* txt, int x, int y, int sz, Color c);

    Color colorNodo(EstNodo s) const;
    Color colorAri(EstAri s) const;
    Color lerpC(Color a, Color b, float t) const;
    float easeIO(float t) const;
    std::string strDst(int d) const;
    bool enRect(Rectangle r, Vector2 p) const;
    bool enCirc(Vector2 center, float r, Vector2 p) const;
};
