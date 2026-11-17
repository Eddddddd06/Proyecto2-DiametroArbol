# Diámetro de un Árbol — CS2023 AED | UTEC 2026-2

> **Proyecto Final 2 – Anima tu Algoritmo o Estructura de Datos**
> Tema asignado: **Diámetro de un árbol**

## Descripción

El **diámetro de un árbol** es el camino de mayor longitud entre dos nodos del árbol, medido como la cantidad de aristas (árbol no ponderado) o la suma de pesos de las aristas (árbol ponderado). Este proyecto implementa el algoritmo clásico de **dos recorridos BFS** en tiempo **O(n)** e incluye una interfaz visual animada en terminal para observar la ejecución paso a paso.

## Algoritmo

1. **BFS 1:** Desde cualquier nodo (p. ej. nodo 0), encontrar el nodo más lejano `a`.
2. **BFS 2:** Desde `a`, encontrar el nodo más lejano `b`.
3. El camino `a → b` es el diámetro del árbol.

**Complejidad:** O(n) tiempo · O(n) espacio

## Estructura del repositorio

```
Proyecto2/
├── src/
│   ├── tree_diameter.h   # Implementación del algoritmo (header-only)
│   └── main.cpp          # Interfaz visual interactiva (terminal)
├── docs/
│   └── CS2023_Proyecto_de_Curso_2.md
├── latex_doc/
│   └── informe.txt       # Código LaTeX del informe (en formato .txt)
└── README.md
```

## Requisitos

- **Compilador:** g++ con soporte C++17 (`-std=c++17`)
- **Sistema operativo:** Linux / macOS (colores ANSI) — en Windows usar WSL o terminal con soporte ANSI

## Compilar y ejecutar

```bash
cd src
g++ -std=c++17 -O2 -o visualizer main.cpp
./visualizer
```

## Casos de prueba incluidos

| # | Descripción                          | n | Diámetro |
|---|--------------------------------------|---|----------|
| 1 | Árbol lineal (cadena)                | 6 | 5        |
| 2 | Árbol tipo Y con rama extra          | 7 | 4        |
| 3 | Árbol ponderado con ramas largas     | 6 | variable |
| 4 | Caso borde: un solo nodo             | 1 | 0        |
| 5 | Caso borde: dos nodos                | 2 | 1        |
| 6 | Entrada personalizada por el usuario | — | —        |

## Interfaz visual

La interfaz muestra en tiempo real:
- Árbol con colores: nodo actual (azul), camino del diámetro (verde)
- Tabla de distancias actualizada en cada paso de BFS
- Fase actual (BFS1 / BFS2) y nodo más lejano encontrado
- Resultado final con el camino del diámetro resaltado
- Análisis de complejidad al final

## Integrantes

- Eduardo (Eddddddd06)

## Curso

CS2023 – Algoritmos y Estructuras de Datos | UTEC 2026-2
