#!/bin/bash
# ============================================================
#  build.sh  –  Compilación directa con g++ (sin CMake)
#  Requiere: raylib instalado  (sudo pacman -S raylib)
#
#  Uso:   chmod +x build.sh && ./build.sh
# ============================================================

set -e
cd "$(dirname "$0")"

echo "==> Compilando Diámetro de un Árbol..."

g++ -std=c++17 -O2 \
    src/main.cpp \
    src/Tree.cpp \
    src/Visualizer.cpp \
    -Isrc \
    $(pkg-config --cflags --libs raylib) \
    -lm -lpthread -ldl \
    -o DiametroArbol

echo "==> Compilado OK: ./DiametroArbol"
echo "==> Ejecuta: ./DiametroArbol"
