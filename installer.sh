#!/bin/bash

set -e

APP_NAME="argentum"

INSTALL_BIN="$HOME/.local/bin"
INSTALL_SHARE="$HOME/.local/share/$APP_NAME"
INSTALL_CONFIG="$HOME/.config/$APP_NAME"

echo "Instalando dependencias del sistema para Argentum..."

sudo apt-get update || echo "WARNING: apt-get update falló, continúo igual..."

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

echo "Ejecutando tests..."
ctest --test-dir build --output-on-failure

echo "Instalando archivos..."
mkdir -p "$INSTALL_BIN"
mkdir -p "$INSTALL_SHARE"
mkdir -p "$INSTALL_CONFIG"

cp build/taller_client "$INSTALL_SHARE/"
cp build/taller_server "$INSTALL_SHARE/"
cp -r assets "$INSTALL_SHARE/"
cp config.toml "$INSTALL_CONFIG/"
cp config.toml "$INSTALL_SHARE/"

cat > "$INSTALL_BIN/argentum-client" <<EOF
#!/bin/bash
cd "$INSTALL_SHARE"
exec ./taller_client "\$@"
EOF

cat > "$INSTALL_BIN/argentum-server" <<EOF
#!/bin/bash
cd "$INSTALL_SHARE"
exec ./taller_server "\$@"
EOF

chmod +x "$INSTALL_BIN/argentum-client"
chmod +x "$INSTALL_BIN/argentum-server"

echo "Instalación finalizada correctamente."
echo "Cliente: argentum-client"
echo "Servidor: argentum-server"
echo "Si no reconoce el comando, agregá ~/.local/bin al PATH:"
echo 'export PATH="$HOME/.local/bin:$PATH"'
