#include "Tree.hpp"
#include <algorithm>
#include <stdexcept>
#include <sstream>

// ============================================================
//  Tree.cpp  –  Implementación del árbol y doble BFS con steps
// ============================================================

Tree::Tree() : n(0) {}

Tree::Tree(int nodes) : n(nodes), adj(nodes) {}

void Tree::clear() {
    n = 0;
    adj.clear();
    edgeList.clear();
    edgeWeights.clear();
}

void Tree::resize(int nodes) {
    n = nodes;
    adj.assign(nodes, {});
    edgeList.clear();
    edgeWeights.clear();
}

void Tree::addEdge(int u, int v, int w) {
    adj[u].push_back({v, w});
    adj[v].push_back({u, w});
    int a = std::min(u, v), b = std::max(u, v);
    edgeList.push_back({a, b});
    edgeWeights.push_back(w);
}

// ── Busca el índice de la arista (u,v) en edgeList ───────────
static int findEdgeIdx(const std::vector<std::pair<int,int>>& edgeList, int u, int v) {
    int a = std::min(u,v), b = std::max(u,v);
    for (int i = 0; i < (int)edgeList.size(); i++)
        if (edgeList[i].first == a && edgeList[i].second == b) return i;
    return -1;
}

// ── Convierte una queue a vector (front primero) ─────────────
static std::vector<int> queueToVec(std::queue<int> q) {
    std::vector<int> v;
    while (!q.empty()) { v.push_back(q.front()); q.pop(); }
    return v;
}

// ── Implementación del BFS que graba micro-pasos ─────────────
Tree::BFSResult Tree::runBFS(int start, BFSPhase phase,
                              std::vector<AlgoStep>& steps) const {
    const std::string phaseTag = (phase == BFSPhase::BFS1) ? "BFS 1" : "BFS 2";
    const int E = (int)edgeList.size();

    std::vector<int> dist(n, -1);
    std::vector<int> parent(n, -1);
    std::vector<NodeState> ns(n, NodeState::UNVISITED);
    std::vector<EdgeState> es(E, EdgeState::UNVISITED);

    std::queue<int> q;
    dist[start] = 0;
    ns[start]   = NodeState::IN_QUEUE;
    q.push(start);

    int farthest     = start;
    int farthestDist = 0;
    int globalStep   = 1;

    // ── Paso INIT ─────────────────────────────────────────────
    {
        AlgoStep s;
        s.type         = StepType::BFS_INIT;
        s.phase        = phase;
        s.u            = start;  s.v = -1;  s.dist_v = 0;
        s.farthest     = start;  s.farthestDist = 0;
        s.queueState   = {start};
        s.distArr      = dist;
        s.parentArr    = parent;
        s.nodeStates   = ns;
        s.edgeStates   = es;
        s.reconstructIdx = -1;
        s.description  = phaseTag + ": Inicializamos el BFS. Encolamos v"
                       + std::to_string(start)
                       + " con dist[v" + std::to_string(start) + "] = 0."
                       + "  Cola = { v" + std::to_string(start) + " }.";
        steps.push_back(s);
    }

    // ── BFS principal ──────────────────────────────────────────
    while (!q.empty()) {
        int u = q.front(); q.pop();
        ns[u] = NodeState::CURRENT;

        // --- Paso DEQUEUE ---
        {
            AlgoStep s;
            s.type         = StepType::BFS_DEQUEUE;
            s.phase        = phase;
            s.u            = u;  s.v = -1;  s.dist_v = -1;
            s.farthest     = farthest;  s.farthestDist = farthestDist;
            s.queueState   = queueToVec(q);
            s.distArr      = dist;  s.parentArr = parent;
            s.nodeStates   = ns;    s.edgeStates = es;
            s.reconstructIdx = -1;
            s.description  = phaseTag + " - Paso " + std::to_string(globalStep)
                           + ": Sacamos v" + std::to_string(u)
                           + " de la cola. dist[v" + std::to_string(u)
                           + "] = " + std::to_string(dist[u])
                           + ". Exploramos sus vecinos...";
            steps.push_back(s);
        }

        // --- Explorar vecinos ---
        for (auto& e : adj[u]) {
            int v = e.to;
            if (dist[v] == -1) {
                dist[v]   = dist[u] + e.weight;
                parent[v] = u;
                ns[v]     = NodeState::IN_QUEUE;
                q.push(v);

                int eIdx = findEdgeIdx(edgeList, u, v);
                if (eIdx >= 0) es[eIdx] = EdgeState::TREE_EDGE;

                if (dist[v] > farthestDist) {
                    farthest     = v;
                    farthestDist = dist[v];
                }

                AlgoStep s;
                s.type        = StepType::BFS_ENQUEUE;
                s.phase       = phase;
                s.u           = u;  s.v = v;  s.dist_v = dist[v];
                s.farthest    = farthest;  s.farthestDist = farthestDist;
                s.queueState  = queueToVec(q);
                s.distArr     = dist;  s.parentArr = parent;
                s.nodeStates  = ns;    s.edgeStates = es;
                s.reconstructIdx = -1;
                s.description = phaseTag + " - Paso " + std::to_string(globalStep)
                              + ": Vecino no visitado v" + std::to_string(v)
                              + ". dist[v" + std::to_string(v)
                              + "] = dist[v" + std::to_string(u)
                              + "] + " + std::to_string(e.weight)
                              + " = " + std::to_string(dist[v])
                              + ". Encolado. Cola = { "
                              + [&]{ std::string r; for(int x:queueToVec(q)) r+="v"+std::to_string(x)+" "; return r; }()
                              + "}.";
                steps.push_back(s);

            } else {
                // Vecino ya visitado – paso SKIP
                AlgoStep s;
                s.type        = StepType::BFS_SKIP;
                s.phase       = phase;
                s.u           = u;  s.v = v;  s.dist_v = dist[v];
                s.farthest    = farthest;  s.farthestDist = farthestDist;
                s.queueState  = queueToVec(q);
                s.distArr     = dist;  s.parentArr = parent;
                s.nodeStates  = ns;    s.edgeStates = es;
                s.reconstructIdx = -1;
                s.description = phaseTag + " - Paso " + std::to_string(globalStep)
                              + ": Vecino v" + std::to_string(v)
                              + " ya visitado (dist = " + std::to_string(dist[v])
                              + "). Lo ignoramos.";
                steps.push_back(s);
            }
        }

        ns[u] = NodeState::PROCESSED;
        globalStep++;
    }

    // ── Paso BFS_DONE ──────────────────────────────────────────
    NodeState extremoState = (phase == BFSPhase::BFS1) ? NodeState::EXTREMO_A : NodeState::EXTREMO_B;
    ns[farthest] = extremoState;
    std::string doneName = (phase == BFSPhase::BFS1) ? "Extremo A" : "Extremo B";
    {
        AlgoStep s;
        s.type        = StepType::BFS_DONE;
        s.phase       = phase;
        s.u           = farthest;  s.v = -1;  s.dist_v = farthestDist;
        s.farthest    = farthest;  s.farthestDist = farthestDist;
        s.queueState  = {};
        s.distArr     = dist;  s.parentArr = parent;
        s.nodeStates  = ns;    s.edgeStates = es;
        s.reconstructIdx = -1;
        s.description = phaseTag + " completado. Nodo más lejano: v"
                      + std::to_string(farthest)
                      + " con dist = " + std::to_string(farthestDist)
                      + ". ¡Este es el " + doneName + "!";
        steps.push_back(s);
    }

    return {farthest, farthestDist, dist, parent};
}

// ── computeSteps: orquesta los dos BFS y la reconstrucción ────
std::vector<AlgoStep> Tree::computeSteps(int startNode) const {
    std::vector<AlgoStep> steps;
    const int E = (int)edgeList.size();

    // ── Caso árbol vacío ──────────────────────────────────────
    if (n == 0) {
        AlgoStep s;
        s.type = StepType::ALGO_DONE;  s.phase = BFSPhase::DONE;
        s.u = -1;  s.v = -1;  s.dist_v = 0;
        s.farthest = -1;  s.farthestDist = 0;
        s.reconstructIdx = -1;
        s.description = "Árbol vacío (N = 0). No hay nodos. Diámetro = 0.";
        steps.push_back(s);
        return steps;
    }

    // ── Caso un solo nodo ─────────────────────────────────────
    if (n == 1) {
        std::vector<NodeState> ns = {NodeState::ON_DIAMETER};
        AlgoStep s;
        s.type = StepType::ALGO_DONE;  s.phase = BFSPhase::DONE;
        s.u = 0;  s.v = 0;  s.dist_v = 0;
        s.farthest = 0;  s.farthestDist = 0;
        s.distArr = {0};  s.parentArr = {-1};
        s.nodeStates = ns;  s.edgeStates = {};
        s.reconstructIdx = 0;
        s.description = "Árbol de un solo nodo (N = 1). Extremo A = Extremo B = v0. Diámetro = 0.";
        steps.push_back(s);
        return steps;
    }

    // ── Paso ALGO_INIT ────────────────────────────────────────
    {
        std::vector<NodeState> ns(n, NodeState::UNVISITED);
        AlgoStep s;
        s.type = StepType::ALGO_INIT;  s.phase = BFSPhase::BFS1;
        s.u = startNode;  s.v = -1;  s.dist_v = -1;
        s.farthest = startNode;  s.farthestDist = 0;
        s.distArr.assign(n, -1);
        s.parentArr.assign(n, -1);
        s.nodeStates = ns;
        s.edgeStates.assign(E, EdgeState::UNVISITED);
        s.reconstructIdx = -1;
        s.description = "Iniciando algoritmo de Doble BFS desde v"
                      + std::to_string(startNode)
                      + ". BFS 1: encontraremos el nodo más lejano (Extremo A).";
        steps.push_back(s);
    }

    // ── BFS 1 ─────────────────────────────────────────────────
    BFSResult r1 = runBFS(startNode, BFSPhase::BFS1, steps);

    // ── BFS 2 ─────────────────────────────────────────────────
    {   // transición
        AlgoStep& prev = steps.back();
        AlgoStep s = prev;
        s.type = StepType::BFS_INIT;  s.phase = BFSPhase::BFS2;
        s.u = r1.extremo;
        s.description = "Iniciando BFS 2 desde el Extremo A (v"
                      + std::to_string(r1.extremo)
                      + "). Reiniciamos distancias. Encontraremos el Extremo B.";
        steps.push_back(s);
    }
    BFSResult r2 = runBFS(r1.extremo, BFSPhase::BFS2, steps);

    // ── Reconstrucción del camino ──────────────────────────────
    std::vector<int> path;
    for (int v = r2.extremo; v != -1; v = r2.parent[v]) path.push_back(v);
    std::reverse(path.begin(), path.end()); // A → B

    int diameter = r2.dist[r2.extremo];

    // Estado base para reconstrucción: todos PROCESSED
    std::vector<NodeState> nsR(n, NodeState::PROCESSED);
    nsR[r1.extremo] = NodeState::EXTREMO_A;
    nsR[r2.extremo] = NodeState::EXTREMO_B;
    std::vector<EdgeState> esR(E, EdgeState::TREE_EDGE);

    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int ua = path[i], va = path[i+1];
        nsR[ua] = NodeState::ON_DIAMETER;
        nsR[va] = NodeState::ON_DIAMETER;
        int eIdx = findEdgeIdx(edgeList, ua, va);
        if (eIdx >= 0) esR[eIdx] = EdgeState::ON_DIAMETER;

        AlgoStep s;
        s.type = StepType::RECONSTRUCT;  s.phase = BFSPhase::RECONSTRUCT;
        s.u = ua;  s.v = va;  s.dist_v = -1;
        s.farthest = r2.extremo;  s.farthestDist = diameter;
        s.queueState = {};
        s.distArr = r2.dist;  s.parentArr = r2.parent;
        s.nodeStates = nsR;    s.edgeStates = esR;
        s.reconstructIdx = i;
        s.description = "Reconstruyendo diámetro: Resaltando arista v"
                      + std::to_string(ua) + " ─ v" + std::to_string(va)
                      + "  (parent[v" + std::to_string(va)
                      + "] = v" + std::to_string(ua) + ")."
                      + "  Arista " + std::to_string(i+1) + " de "
                      + std::to_string((int)path.size()-1) + ".";
        steps.push_back(s);
    }

    // ── ALGO_DONE ──────────────────────────────────────────────
    {
        std::string pathStr = "v" + std::to_string(path[0]);
        for (int i = 1; i < (int)path.size(); i++)
            pathStr += " → v" + std::to_string(path[i]);

        AlgoStep s;
        s.type = StepType::ALGO_DONE;  s.phase = BFSPhase::DONE;
        s.u = r1.extremo;  s.v = r2.extremo;  s.dist_v = diameter;
        s.farthest = r2.extremo;  s.farthestDist = diameter;
        s.queueState = {};
        s.distArr = r2.dist;  s.parentArr = r2.parent;
        s.nodeStates = nsR;   s.edgeStates = esR;
        s.reconstructIdx = (int)path.size() - 1;
        s.description = "¡Algoritmo completo! Camino del diámetro: "
                      + pathStr
                      + ".  Longitud del diámetro = "
                      + std::to_string(diameter) + " arista(s).";
        steps.push_back(s);
    }

    return steps;
}

// ── extractResult ─────────────────────────────────────────────
Tree::Result Tree::extractResult(const std::vector<AlgoStep>& steps) {
    Result r;
    if (steps.empty()) return r;
    const AlgoStep& last = steps.back();
    if (last.type != StepType::ALGO_DONE) return r;

    r.valid    = true;
    r.diameter = last.dist_v;
    r.extremoA = last.u;
    r.extremoB = last.v;

    // Reconstruir path desde parentArr en el último paso
    if (last.v != -1 && !last.parentArr.empty()) {
        for (int v = last.v; v != -1; v = last.parentArr[v])
            r.path.push_back(v);
        std::reverse(r.path.begin(), r.path.end());
    } else if (last.u == last.v && last.u != -1) {
        r.path = {last.u};
    }
    return r;
}
