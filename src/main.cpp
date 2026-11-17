// ============================================================
//  main.cpp  –  Interfaz visual interactiva (terminal)
//  Diámetro de un Árbol  |  CS2023 AED  |  UTEC 2026-2
//
//  Compilar:  g++ -std=c++17 -O2 -o visualizer main.cpp
//  Ejecutar:  ./visualizer
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "tree_diameter.h"

// ── Colores ANSI ────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BG_BLUE "\033[44m"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause(int ms = 700) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// ── Banner principal ────────────────────────────────────────
void printBanner() {
    std::cout << BOLD << CYAN;
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════════════╗\n";
    std::cout << "  ║      DIÁMETRO DE UN ÁRBOL  –  CS2023 AED        ║\n";
    std::cout << "  ║          Algoritmo: dos BFS  |  O(n)            ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
}

// ── Muestra las distancias en modo tabla ────────────────────
void printDistTable(const std::vector<int>& dist, int highlight,
                    const std::string& phase) {
    std::cout << "  " << BOLD << YELLOW << "[" << phase << "] " << RESET
              << "Distancias desde origen:\n";
    std::cout << "  ";
    for (int i = 0; i < (int)dist.size(); i++) {
        if (i == highlight)
            std::cout << BG_BLUE << WHITE << BOLD;
        std::cout << " v" << i << ":";
        if (dist[i] == -1) std::cout << " ?";
        else                std::cout << std::setw(2) << dist[i];
        if (i == highlight)
            std::cout << RESET;
        std::cout << " |";
    }
    std::cout << "\n";
}

// ── Dibuja el árbol en ASCII (lista de adyacencia) ─────────
void printTree(const TreeDiameter& td, const std::vector<int>& diamPath,
               int currentNode = -1) {
    std::cout << "\n" << BOLD << "  Árbol (lista de adyacencia):\n" << RESET;
    for (int u = 0; u < td.n; u++) {
        // Color del nodo
        bool inPath = false;
        for (int v : diamPath) if (v == u) { inPath = true; break; }

        if (u == currentNode)
            std::cout << "  " << BOLD << BG_BLUE << WHITE;
        else if (inPath)
            std::cout << "  " << BOLD << GREEN;
        else
            std::cout << "  " << WHITE;

        std::cout << "  v" << u << RESET << " ──► ";
        for (auto [v, w] : td.adj[u]) {
            bool vInPath = false;
            for (int p : diamPath) if (p == v) { vInPath = true; break; }
            if (vInPath && inPath)
                std::cout << BOLD << GREEN;
            std::cout << "v" << v << "(w=" << w << ") " << RESET;
        }
        std::cout << "\n";
    }
}

// ── Muestra el camino del diámetro coloreado ────────────────
void printDiamPath(const std::vector<int>& path, int diameter) {
    std::cout << "\n  " << BOLD << GREEN
              << "◉ DIÁMETRO = " << diameter << " aristas/peso" << RESET << "\n";
    std::cout << "  Camino: ";
    for (int i = 0; i < (int)path.size(); i++) {
        if (i > 0) std::cout << BOLD << GREEN << " ──► " << RESET;
        std::cout << BOLD << GREEN << "v" << path[i] << RESET;
    }
    std::cout << "\n\n";
}

// ── Animación paso a paso ───────────────────────────────────
void animate(TreeDiameter& td, const std::vector<int>& diamPath, int diameter,
             int nodeA, int nodeB, int delayMs) {
    for (const auto& step : td.steps) {
        clearScreen();
        printBanner();
        printTree(td, diamPath, step.current);
        std::cout << "\n";
        printDistTable(step.dist, step.current, step.phase);
        std::cout << "\n  " << CYAN
                  << "  Procesando: v" << step.current
                  << "   |   Nodo más lejano hasta ahora: v" << step.farthest
                  << RESET << "\n";
        pause(delayMs);
    }

    // Pantalla final
    clearScreen();
    printBanner();
    printTree(td, diamPath, -1);
    printDiamPath(diamPath, diameter);
    std::cout << "  " << BOLD << YELLOW
              << "  Nodo extremo A: v" << nodeA
              << "   |   Nodo extremo B: v" << nodeB << RESET << "\n\n";
}

// ── Menú principal ──────────────────────────────────────────
int main() {
    clearScreen();
    printBanner();

    // ── Selección de caso de prueba ──────────────────────────
    std::cout << BOLD << "  Selecciona un caso de prueba:\n" << RESET;
    std::cout << "  [1] Árbol lineal  (n=6, no ponderado)   – diámetro = 5\n";
    std::cout << "  [2] Árbol tipo Y  (n=7, no ponderado)   – diámetro = 4\n";
    std::cout << "  [3] Árbol ponderado con ramas largas    – diámetro variable\n";
    std::cout << "  [4] Árbol trivial (n=1)  – caso borde\n";
    std::cout << "  [5] Árbol con dos nodos  – caso borde\n";
    std::cout << "  [6] Ingresa tu propio árbol\n";
    std::cout << "\n  Opción: ";

    int op;
    std::cin >> op;

    int nodes;
    std::vector<std::tuple<int,int,int>> edges; // (u, v, peso)

    switch (op) {
        case 1:
            // 0-1-2-3-4-5
            nodes = 6;
            edges = {{0,1,1},{1,2,1},{2,3,1},{3,4,1},{4,5,1}};
            break;
        case 2:
            //       0
            //      / \
            //     1   2
            //    / \   \
            //   3   4   5
            //            \
            //             6
            nodes = 7;
            edges = {{0,1,1},{0,2,1},{1,3,1},{1,4,1},{2,5,1},{5,6,1}};
            break;
        case 3:
            // Ponderado:  0-(3)-1-(1)-2  , 0-(5)-3-(2)-4-(4)-5
            nodes = 6;
            edges = {{0,1,3},{1,2,1},{0,3,5},{3,4,2},{4,5,4}};
            break;
        case 4:
            // Caso borde: solo un nodo
            nodes = 1;
            break;
        case 5:
            // Caso borde: dos nodos
            nodes = 2;
            edges = {{0,1,1}};
            break;
        case 6: {
            std::cout << "\n  Número de nodos: ";
            std::cin >> nodes;
            int e;
            std::cout << "  Número de aristas: ";
            std::cin >> e;
            std::cout << "  Ingresa cada arista como  'u v peso'  (0-indexed):\n";
            for (int i = 0; i < e; i++) {
                int u, v, w;
                std::cout << "  Arista " << i+1 << ": ";
                std::cin >> u >> v >> w;
                edges.push_back({u, v, w});
            }
            break;
        }
        default:
            std::cout << "  Opción inválida, usando caso 1.\n";
            nodes = 6;
            edges = {{0,1,1},{1,2,1},{2,3,1},{3,4,1},{4,5,1}};
    }

    // ── Velocidad de animación ───────────────────────────────
    std::cout << "\n  Velocidad de animación:\n";
    std::cout << "  [1] Rápida (300ms)   [2] Normal (700ms)   [3] Lenta (1500ms)\n";
    std::cout << "  Opción: ";
    int speed; std::cin >> speed;
    int delayMs = (speed == 1) ? 300 : (speed == 3) ? 1500 : 700;

    // ── Construir árbol y calcular diámetro ──────────────────
    TreeDiameter td(nodes);
    for (auto [u, v, w] : edges) td.addEdge(u, v, w);

    auto [diameter, nodeA, nodeB, path] = td.computeDiameter();

    // ── Animar ───────────────────────────────────────────────
    animate(td, path, diameter, nodeA, nodeB, delayMs);

    // ── Análisis de complejidad ──────────────────────────────
    std::cout << BOLD << MAGENTA
              << "  ╔══════════ ANÁLISIS DE COMPLEJIDAD ══════════╗\n"
              << "  ║  Tiempo:  O(n)  –  2 recorridos BFS        ║\n"
              << "  ║  Espacio: O(n)  –  arreglos dist y padre   ║\n"
              << "  ║  n = número de nodos del árbol             ║\n"
              << "  ╚═════════════════════════════════════════════╝\n"
              << RESET << "\n";

    std::cout << "  Presiona Enter para salir...";
    std::cin.ignore(); std::cin.get();
    return 0;
}
