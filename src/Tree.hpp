#pragma once
#include <vector>
#include <string>
#include <utility>
#include <queue>

// ============================================================
//  Tree.hpp  -  Árbol + Doble BFS con grabación de micro-pasos
//  CS2023 AED | UTEC 2026-2  |  "Diámetro de un Árbol"
// ============================================================

// ── Estado visual de cada nodo ───────────────────────────────
enum class NodeState {
    UNVISITED,    // #3B4252 gris azulado – no descubierto
    IN_QUEUE,     // #EBCB8B ámbar       – en la cola BFS
    CURRENT,      // #88C0D0 cian        – recién sacado de la cola
    PROCESSED,    // #5E81AC azul acero  – completamente procesado
    EXTREMO_A,    // #BF616A rojo coral  – extremo hallado en BFS1
    EXTREMO_B,    // #BF616A rojo coral  – extremo hallado en BFS2
    ON_DIAMETER   // #A3BE8C verde       – en el camino del diámetro
};

// ── Estado visual de cada arista ─────────────────────────────
enum class EdgeState {
    UNVISITED,    // gris apagado
    TREE_EDGE,    // azul acero – arista del árbol BFS (no en diámetro)
    ON_DIAMETER   // verde/dorado – en el camino del diámetro
};

// ── Fase del algoritmo ────────────────────────────────────────
enum class BFSPhase {
    IDLE,
    BFS1,
    BFS2,
    RECONSTRUCT,
    DONE
};

// ── Tipo de micro-paso ────────────────────────────────────────
enum class StepType {
    ALGO_INIT,    // algoritmo comienza
    BFS_INIT,     // BFS inicializado desde un nodo
    BFS_DEQUEUE,  // nodo u sacado de la cola
    BFS_ENQUEUE,  // vecino v encolado
    BFS_SKIP,     // vecino v ya visitado (ignorado)
    BFS_DONE,     // BFS terminó; extremo encontrado
    RECONSTRUCT,  // resaltando una arista del camino del diámetro
    ALGO_DONE     // algoritmo completo
};

// ── Micro-paso de animación ───────────────────────────────────
struct AlgoStep {
    StepType  type;
    BFSPhase  phase;
    int u;                       // nodo principal (sacado de cola / actual)
    int v;                       // vecino procesado (-1 si no aplica)
    int dist_v;                  // distancia asignada a v (-1 si no aplica)
    int farthest;                // nodo más lejano hasta ahora
    int farthestDist;            // distancia al nodo más lejano
    std::vector<int> queueState; // contenido de la cola (frente primero)
    std::vector<int> distArr;    // dist[] completo en este paso
    std::vector<int> parentArr;  // parent[] completo en este paso
    std::vector<NodeState> nodeStates;
    std::vector<EdgeState> edgeStates;
    std::string description;     // texto explicativo para la barra inferior
    int reconstructIdx;          // índice de arista en reconstrucción (-1 si no aplica)
};

// ── Arista interna ────────────────────────────────────────────
struct TreeEdge {
    int to;
    int weight;
};

// ── Árbol principal ───────────────────────────────────────────
class Tree {
public:
    int n;
    std::vector<std::vector<TreeEdge>> adj;

    // Lista plana de aristas (u <= v), para indexar EdgeState
    std::vector<std::pair<int,int>> edgeList;
    std::vector<int>                edgeWeights;

    Tree();
    explicit Tree(int nodes);

    void addEdge(int u, int v, int w = 1);
    void clear();
    void resize(int nodes);

    // Genera todos los micro-pasos para el doble BFS
    // startNode: nodo de partida del BFS1
    std::vector<AlgoStep> computeSteps(int startNode = 0) const;

    // Resultado del diámetro (extraído del último paso ALGO_DONE)
    struct Result {
        int diameter   = 0;
        int extremoA   = -1;
        int extremoB   = -1;
        std::vector<int> path; // de A a B
        bool valid     = false;
    };
    static Result extractResult(const std::vector<AlgoStep>& steps);

private:
    // Helper: corre un BFS desde 'start' grabando pasos; retorna extremo y dist/parent
    struct BFSResult {
        int extremo;
        int extremoDist;
        std::vector<int> dist;
        std::vector<int> parent;
    };
    BFSResult runBFS(int start, BFSPhase phase,
                     std::vector<AlgoStep>& steps) const;
};
