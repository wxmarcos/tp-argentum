#!/bin/bash

set -e

echo "Instalando dependencias del sistema para Argentum..."

sudo apt-get update

sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    gdb \
    valgrind \
    clang-format \
    libpthread-stubs0-dev \
    libopus-dev \
    libopusfile-dev \
    libxmp-dev \
    libfluidsynth-dev \
    fluidsynth \
    libwavpack1 \
    libwavpack-dev \
    wavpack \
    libfreetype-dev \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-ttf-dev \
    libsdl2-gfx-dev \
    qtbase5-dev \
    qt5-qmake \
    qtbase5-dev-tools \
    libyaml-cpp-dev \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libxss-dev \
    libxxf86vm-dev \
    libwayland-dev \
    wayland-protocols \
    libxkbcommon-dev \
    x11-apps

echo "Dependencias instaladas correctamente."

echo "Limpiando build anterior..."
rm -rf build

echo "Configurando CMake..."
cmake -S . -B build

echo "Compilando proyecto..."
cmake --build build

echo "Compilación finalizada correctamente."