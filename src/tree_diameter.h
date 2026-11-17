#pragma once
#include <vector>
#include <queue>
#include <utility>
#include <climits>
#include <functional>
#include <algorithm>

// ============================================================
//  Tree Diameter - Core Implementation
//  Algoritmo: dos BFS/DFS para encontrar el diámetro del árbol
//  Complejidad: O(n) tiempo, O(n) espacio
// ============================================================

struct TreeDiameter {
    int n;                                        // número de nodos (0-indexed)
    std::vector<std::vector<std::pair<int,int>>> adj; // adj[u] = {(v, peso)}

    // Pasos de animación guardados para visualización
    struct Step {
        std::string phase;   // "BFS1" o "BFS2"
        int current;         // nodo actual siendo procesado
        std::vector<int> dist;  // distancias actuales
        std::vector<int> path;  // camino actual al nodo más lejano
        int farthest;        // nodo más lejano encontrado hasta ahora
    };
    std::vector<Step> steps;

    explicit TreeDiameter(int nodes) : n(nodes), adj(nodes) {}

    // Agrega arista no dirigida con peso (default=1 para árbol no ponderado)
    void addEdge(int u, int v, int w = 1) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    // BFS desde 'start', retorna (nodo_mas_lejano, distancias, padre)
    // Registra pasos para animación
    std::tuple<int, std::vector<int>, std::vector<int>>
    bfs(int start, const std::string& phase) {
        std::vector<int> dist(n, -1);
        std::vector<int> parent(n, -1);
        std::queue<int> q;
        dist[start] = 0;
        q.push(start);

        int farthest = start;

        while (!q.empty()) {
            int u = q.front(); q.pop();

            // Guardar paso
            Step s;
            s.phase    = phase;
            s.current  = u;
            s.dist     = dist;
            s.farthest = farthest;
            steps.push_back(s);

            for (auto [v, w] : adj[u]) {
                if (dist[v] == -1) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    q.push(v);
                    if (dist[v] > dist[farthest]) farthest = v;
                }
            }
        }
        return {farthest, dist, parent};
    }

    // Reconstruye el camino desde 'end' hasta 'start' usando el arreglo de padres
    std::vector<int> reconstructPath(int start, int end,
                                     const std::vector<int>& parent) {
        std::vector<int> path;
        for (int v = end; v != -1; v = parent[v])
            path.push_back(v);
        std::reverse(path.begin(), path.end());
        return path;
    }

    // Calcula el diámetro del árbol (dos BFS)
    // Retorna: (longitud_diámetro, nodo_a, nodo_b, camino_diámetro)
    std::tuple<int, int, int, std::vector<int>> computeDiameter() {
        steps.clear();

        // BFS 1: desde nodo 0, encontrar el nodo más lejano 'a'
        auto [a, dist1, parent1] = bfs(0, "BFS1");

        // BFS 2: desde 'a', encontrar el nodo más lejano 'b'
        auto [b, dist2, parent2] = bfs(a, "BFS2");

        int diameter = dist2[b];
        std::vector<int> path = reconstructPath(a, b, parent2);

        return {diameter, a, b, path};
    }
};
