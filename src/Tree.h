#pragma once
#include <vector>
#include <string>
#include <utility>
#include <queue>


// Los enumss  
enum class EstNodo {
    NO_VIS,
    EN_COLA,
    ACTUAL,
    PROCESADO,
    EXTREMO_A,
    EXTREMO_B,
    EN_DIAM
};

enum class EstAri {
    NO_VIS,
    ARI_BFS,
    EN_DIAM
};

enum class FaseBFS {
    IDLE,
    BFS1,
    BFS2,
    RECONSTRUCT,
    DONE
};

enum class TipoPaso {
    ALGO_INI,
    BFS_INI,
    BFS_SACA,
    BFS_METE,
    BFS_SALTA,
    BFS_FIN,
    RECONST,
    ALGO_FIN
};

// los structs 

struct Paso {
    TipoPaso tipo;
    FaseBFS fase;
    int u;
    int v;
    int dst_v;
    int lejano;
    int dstMax;
    std::vector<int> estCola;
    std::vector<int> arrDst;
    std::vector<int> arrPad;
    std::vector<EstNodo> estNodos;
    std::vector<EstAri> estAris;
    std::string desc;
    int idxRec;
};

struct Arista {
    int dest;
    int peso;
};


// clase tree base 
class Tree {
public:
    int n;
    std::vector<std::vector<Arista>>  adj; // lsita de adyacencia  
    std::vector<std::pair<int,int>> listaAri;  // tupla 
    std::vector<int>  pesosAri ; // lista de pesos 
    Tree();
    explicit Tree(int nodes); 
    void agregarAri(int u,int v,int w = 1);
    void clear();
    void resize(int nodes); 
    std::vector<Paso> calcPasos(int nodoIni = 0)  const;


    struct Result {
        int diam =0;
        int extremoA  =-1;
        int extremoB= -1 ;
        std::vector<int>  camino;
        bool ok = false ;
    };

    static Result sacarRes(const std::vector<Paso>&  pasos);

private:

    struct ResBFS {
        int extremo;
        int dstExtr;
        std::vector<int> dst;
        std::vector<int> pad;
    };

    ResBFS runBFS(int start, FaseBFS fase, std::vector<Paso>& pasos) const;
};
