#include "Tree.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>

Tree::Tree() : n(0) {}

Tree::Tree(int nodes) : n(nodes), adj(nodes) {}

void Tree::clear() {
    n = 0;
    adj.clear();
    listaAri.clear();
    pesosAri.clear();
}

void Tree::resize(int nodes) {
    n = nodes;
    adj.assign(nodes, {});
    listaAri.clear();
    pesosAri.clear();
}

void Tree::agregarAri(int u, int v, int w) {
    adj[u].push_back({v, w});
    adj[v].push_back({u, w});
    int a = std::min(u, v), b = std::max(u, v);
    listaAri.push_back({a, b});
    pesosAri.push_back(w);
}

static int buscarAri(const std::vector<std::pair<int,int>>& lista, int u, int v) {
    int a = std::min(u,v), b = std::max(u,v);
    for (int i = 0; i < (int)lista.size(); ++i)
        if (lista[i].first == a && lista[i].second == b) return i;
    return -1;
}

static std::vector<int> colaAVec(std::queue<int> q) {
    std::vector<int> v;
    while (!q.empty()) { v.push_back(q.front()); q.pop(); }
    return v;
}

Tree::ResBFS Tree::runBFS(int start, FaseBFS fase,
                           std::vector<Paso>& pasos) const {
    const std::string tag = (fase == FaseBFS::BFS1) ? "BFS 1" : "BFS 2";
    const int E = (int)listaAri.size();

    std::vector<int> dst(n, -1);
    std::vector<int> pad(n, -1);
    std::vector<EstNodo> ns(n, EstNodo::NO_VIS);
    std::vector<EstAri> es(E, EstAri::NO_VIS);

    std::queue<int> q;
    dst[start] = 0;
    ns[start] = EstNodo::EN_COLA;
    q.push(start);

    int lejano = start;
    int dstMax = 0;
    int numPaso = 1;

    {
        Paso s;
        s.tipo = TipoPaso::BFS_INI;
        s.fase = fase;
        s.u = start; s.v = -1; s.dst_v = 0;
        s.lejano = start; s.dstMax = 0;
        s.estCola = {start};
        s.arrDst = dst;
        s.arrPad = pad;
        s.estNodos = ns;
        s.estAris = es;
        s.idxRec = -1;
        s.desc = tag + ": Inicializamos el BFS. Encolamos v"
               + std::to_string(start)
               + " con dist[v" + std::to_string(start) + "] = 0."
               + "  Cola = { v" + std::to_string(start) + " }.";
        pasos.push_back(s);
    }

    while (!q.empty()) {
        int u = q.front(); q.pop();
        ns[u] = EstNodo::ACTUAL;

        {
            Paso s;
            s.tipo = TipoPaso::BFS_SACA;
            s.fase = fase;
            s.u = u; s.v = -1; s.dst_v = -1;
            s.lejano = lejano; s.dstMax = dstMax;
            s.estCola = colaAVec(q);
            s.arrDst = dst; s.arrPad = pad;
            s.estNodos = ns; s.estAris = es;
            s.idxRec = -1;
            s.desc = tag + " - Paso " + std::to_string(numPaso)
                   + ": Sacamos v" + std::to_string(u)
                   + " de la cola. dist[v" + std::to_string(u)
                   + "] = " + std::to_string(dst[u])
                   + ". Exploramos sus vecinos...";
            pasos.push_back(s);
        }

        for (auto& e : adj[u]) {
            int v = e.dest;
            if (dst[v] == -1) {
                dst[v] = dst[u] + e.peso;
                pad[v] = u;
                ns[v] = EstNodo::EN_COLA;
                q.push(v);

                int eIdx = buscarAri(listaAri, u, v);
                if (eIdx >= 0) es[eIdx] = EstAri::ARI_BFS;

                // sacar max dist
                if (dst[v] > dstMax) {
                    lejano = v;
                    dstMax = dst[v];
                }

                Paso s;
                s.tipo = TipoPaso::BFS_METE;
                s.fase = fase;
                s.u = u; s.v = v; s.dst_v = dst[v];
                s.lejano = lejano; s.dstMax = dstMax;
                s.estCola = colaAVec(q);
                s.arrDst = dst; s.arrPad = pad;
                s.estNodos = ns; s.estAris = es;
                s.idxRec = -1;
                s.desc = tag + " - Paso " + std::to_string(numPaso)
                       + ": Vecino no visitado v" + std::to_string(v)
                       + ". dist[v" + std::to_string(v)
                       + "] = dist[v" + std::to_string(u)
                       + "] + " + std::to_string(e.peso)
                       + " = " + std::to_string(dst[v])
                       + ". Encolado. Cola = { "
                       + [&]{ std::string r; for(int x:colaAVec(q)) r+="v"+std::to_string(x)+" "; return r; }()
                       + "}.";
                pasos.push_back(s);

            } else {
                Paso s;
                s.tipo = TipoPaso::BFS_SALTA;
                s.fase = fase;
                s.u = u; s.v = v; s.dst_v = dst[v];
                s.lejano = lejano; s.dstMax = dstMax;
                s.estCola = colaAVec(q);
                s.arrDst = dst; s.arrPad = pad;
                s.estNodos = ns; s.estAris = es;
                s.idxRec = -1;
                s.desc = tag + " - Paso " + std::to_string(numPaso)
                       + ": Vecino v" + std::to_string(v)
                       + " ya visitado (dist = " + std::to_string(dst[v])
                       + "). Lo ignoramos.";
                pasos.push_back(s);
            }
        }

        ns[u] = EstNodo::PROCESADO;
        numPaso++;
    }

    EstNodo estExtr = (fase == FaseBFS::BFS1) ? EstNodo::EXTREMO_A : EstNodo::EXTREMO_B;
    ns[lejano] = estExtr;
    std::string nombre = (fase == FaseBFS::BFS1) ? "Extremo A" : "Extremo B";
    {
        Paso s;
        s.tipo = TipoPaso::BFS_FIN;
        s.fase = fase;
        s.u = lejano; s.v = -1; s.dst_v = dstMax;
        s.lejano = lejano; s.dstMax = dstMax;
        s.estCola = {};
        s.arrDst = dst; s.arrPad = pad;
        s.estNodos = ns; s.estAris = es;
        s.idxRec = -1;
        s.desc = tag + " completado. Nodo más lejano: v"
               + std::to_string(lejano)
               + " con dist = " + std::to_string(dstMax)
               + ". ¡Este es el " + nombre + "!";
        pasos.push_back(s);
    }

    return {lejano, dstMax, dst, pad};
}


std::vector<Paso> Tree::calcPasos(int nodoIni) const {
    std::vector<Paso> pasos;
    const int E = (int)listaAri.size();

    if (n == 0) {
        Paso s;
        s.tipo = TipoPaso::ALGO_FIN; s.fase = FaseBFS::DONE;
        s.u = -1; s.v = -1; s.dst_v = 0;
        s.lejano = -1; s.dstMax = 0;
        s.idxRec = -1;
        s.desc = "Árbol vacío (N = 0). No hay nodos. Diámetro = 0.";
        pasos.push_back(s);
        return pasos;
    }

    if (n == 1) {
        std::vector<EstNodo> ns = {EstNodo::EN_DIAM};
        Paso s;
        s.tipo = TipoPaso::ALGO_FIN; s.fase = FaseBFS::DONE;
        s.u = 0; s.v = 0; s.dst_v = 0;
        s.lejano = 0; s.dstMax = 0;
        s.arrDst = {0}; s.arrPad = {-1};
        s.estNodos = ns; s.estAris = {};
        s.idxRec = 0;
        s.desc = "Árbol de un solo nodo (N = 1). Extremo A = Extremo B = v0. Diámetro = 0.";
        pasos.push_back(s);
        return pasos;
    }

    {
        std::vector<EstNodo> ns(n, EstNodo::NO_VIS);
        Paso s;
        s.tipo = TipoPaso::ALGO_INI; s.fase = FaseBFS::BFS1;
        s.u = nodoIni; s.v = -1; s.dst_v = -1;
        s.lejano = nodoIni; s.dstMax = 0;
        s.arrDst.assign(n, -1);
        s.arrPad.assign(n, -1);
        s.estNodos = ns;
        s.estAris.assign(E, EstAri::NO_VIS);
        s.idxRec = -1;
        s.desc = "Iniciando algoritmo de Doble BFS desde v"
               + std::to_string(nodoIni)
               + ". BFS 1: encontraremos el nodo más lejano (Extremo A).";
        pasos.push_back(s);
    }

    ResBFS r1 = runBFS(nodoIni, FaseBFS::BFS1, pasos);

    {
        Paso& prev = pasos.back();
        Paso s = prev;
        s.tipo = TipoPaso::BFS_INI; s.fase = FaseBFS::BFS2;
        s.u = r1.extremo;
        s.desc = "Iniciando BFS 2 desde el Extremo A (v"
               + std::to_string(r1.extremo)
               + "). Reiniciamos distancias. Encontraremos el Extremo B.";
        pasos.push_back(s);
    }
    ResBFS r2 = runBFS(r1.extremo, FaseBFS::BFS2, pasos);

    // reconstruir camino
    std::vector<int> camino;
    for (int v = r2.extremo; v != -1; v = r2.pad[v]) camino.push_back(v);
    std::reverse(camino.begin(), camino.end());

    int diam = r2.dst[r2.extremo];

    std::vector<EstNodo> nsr(n, EstNodo::PROCESADO);
    nsr[r1.extremo] = EstNodo::EXTREMO_A;
    nsr[r2.extremo] = EstNodo::EXTREMO_B;
    std::vector<EstAri> esr(E, EstAri::ARI_BFS);

    for (int i = 0; i + 1 < (int)camino.size(); i++) {
        int ua = camino[i], va = camino[i+1];
        nsr[ua] = EstNodo::EN_DIAM;
        nsr[va] = EstNodo::EN_DIAM;
        int eIdx = buscarAri(listaAri, ua, va);
        if (eIdx >= 0) esr[eIdx] = EstAri::EN_DIAM;

        Paso s;
        s.tipo = TipoPaso::RECONST; s.fase = FaseBFS::RECONSTRUCT;
        s.u = ua; s.v = va; s.dst_v = -1;
        s.lejano = r2.extremo; s.dstMax = diam;
        s.estCola = {};
        s.arrDst = r2.dst; s.arrPad = r2.pad;
        s.estNodos = nsr; s.estAris = esr;
        s.idxRec = i;
        s.desc = "Reconstruyendo diámetro: Resaltando arista v"
               + std::to_string(ua) + " ─ v" + std::to_string(va)
               + "  (parent[v" + std::to_string(va)
               + "] = v" + std::to_string(ua) + ")."
               + "  Arista " + std::to_string(i+1) + " de "
               + std::to_string((int)camino.size()-1) + ".";
        pasos.push_back(s);
    }

    {
        std::string caminoStr = "v" + std::to_string(camino[0]);
        for (int i = 1; i < (int)camino.size(); ++i)
            caminoStr += " → v" + std::to_string(camino[i]);

        Paso s;
        s.tipo = TipoPaso::ALGO_FIN; s.fase = FaseBFS::DONE;
        s.u = r1.extremo; s.v = r2.extremo; s.dst_v = diam;
        s.lejano = r2.extremo; s.dstMax = diam;
        s.estCola = {};
        s.arrDst = r2.dst; s.arrPad = r2.pad;
        s.estNodos = nsr; s.estAris = esr;
        s.idxRec = (int)camino.size() - 1;
        s.desc = "¡Algoritmo completo! Camino del diámetro: "
               + caminoStr
               + ".  Longitud del diámetro = "
               + std::to_string(diam) + " arista(s).";
        pasos.push_back(s);
    }

    return pasos;
}

Tree::Result Tree::sacarRes(const std::vector<Paso>& pasos) {
    Result r;
    if (pasos.empty()) return r;
    const Paso& ult = pasos.back();
    if (ult.tipo != TipoPaso::ALGO_FIN) return r;

    r.ok = true;
    r.diam = ult.dst_v;
    r.extremoA = ult.u;
    r.extremoB = ult.v;

    if (ult.v != -1 && !ult.arrPad.empty()) {
        for (int v = ult.v; v != -1; v = ult.arrPad[v])
            r.camino.push_back(v);
        std::reverse(r.camino.begin(), r.camino.end());
    } else if (ult.u == ult.v && ult.u != -1) {
        r.camino = {ult.u};
    }
    return r;
}
