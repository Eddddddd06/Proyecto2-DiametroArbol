# Diámetro de un Árbol — CS2023 AED | UTEC 2026-2

> **Proyecto Final 2 – Anima tu Algoritmo o Estructura de Datos**
> Tema: **Diámetro de un árbol (Doble BFS en O(n))**

## Descripción

Aplicación gráfica interactiva de escritorio en **C++ con Raylib** que visualiza paso a paso el algoritmo de **Doble BFS** para encontrar el diámetro de un árbol en tiempo **O(n)**. Diseñada para ser usada en vivo mientras se graba con OBS para el video del proyecto.

## Estructura del repositorio

```
Proyecto2/
├── src/
│   ├── Tree.h            # Tipos, enums, clase Tree
│   ├── Tree.cpp          # Doble BFS con grabación de micro-pasos
│   ├── Visualizer.h      # Clase Visualizer (Raylib)
│   ├── Visualizer.cpp    # Toda la UI gráfica (~1300 líneas)
│   └── main.cpp          # Punto de entrada
├── docs/
│   └── CS2023_Proyecto_de_Curso_2.md
├── latex_doc/
│   └── informe.txt       # Código LaTeX del informe
├── CMakeLists.txt
├── build.sh              # Script de compilación directa
└── README.md
```

## Requisitos

```bash
sudo pacman -S raylib cmake      # Arch / EndeavourOS
# ó
sudo apt install libraylib-dev cmake  # Ubuntu
```

## Compilar y ejecutar

**Opción 1 – Script directo (más rápido):**
```bash
chmod +x build.sh
./build.sh
./DiametroArbol
```

**Opción 2 – CMake:**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./DiametroArbol
```

**Opción 3 – g++ directo:**
```bash
g++ -std=c++17 -O2 src/main.cpp src/Tree.cpp src/Visualizer.cpp \
    -Isrc $(pkg-config --cflags --libs raylib) -lm -lpthread -ldl \
    -o DiametroArbol
./DiametroArbol
```

## Controles de la aplicación

| Acción | Mouse | Teclado |
|--------|-------|---------|
| Cambiar pestaña | Clic en tab | F1 / F2 / F3 / F4 |
| Play / Pausa | Botón ▶ | Espacio |
| Paso siguiente | Botón ⏭ | → |
| Paso anterior | Botón ⏮ | ← |
| Reiniciar | Botón 🔄 | R |
| Velocidad + / − | Botones | + / − |
| Cambiar árbol ponderado | Botón | W |
| Elegir nodo inicial | Clic en nodo | — |

## Pestañas (F1–F4)

| Tab | Contenido |
|-----|-----------|
| **F1 – Portada** | Título, integrantes, 3 tarjetas conceptuales |
| **F2 – Simulador** | Árbol de 10 nodos, animación completa del Doble BFS |
| **F3 – Casos Borde** | Árbol vacío, 1 nodo, cadena lineal, estrella |
| **F4 – Complejidad** | Comparativa O(n) vs O(n²), desglose, créditos |

## Algoritmo implementado

1. **BFS 1:** Desde nodo inicial X → encuentra el nodo más lejano **A**
2. **BFS 2:** Desde A → encuentra el nodo más lejano **B**
3. El camino **A → B** es el **diámetro** del árbol

**Complejidad:** O(n) tiempo · O(n) espacio

## Código de colores en la visualización

| Color | Significado |
|-------|-------------|
| 🔵 Gris azulado | No visitado |
| 🟡 Ámbar | En la cola BFS |
| 🩵 Cian | Nodo actual (procesando) |
| 🔷 Azul acero | Procesado completamente |
| 🔴 Rojo coral | Extremo A / Extremo B |
| 🟢 Verde | Camino del diámetro final |

## Grabación del video (OBS)

1. Ejecutar `./DiametroArbol`
2. En OBS: capturar ventana específica → `DiametroArbol`
3. Navegar por las 4 pestañas con F1–F4
4. En F2: elegir nodo inicial con clic, usar Play/Paso a paso
5. En F3: mostrar los 4 casos borde
6. Exportar como `.mp4` en alta resolución

## Integrantes

- Eduardo (Eddddddd06) | UTEC
- Leonardo () | UTEC

## Curso

CS2023 – Algoritmos y Estructuras de Datos | UTEC 2026-2 | Prof. Víctor Racsó Galván Oyola
