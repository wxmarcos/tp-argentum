#!/bin/bash

set -e

APP_NAME="argentum"

INSTALL_BIN="$HOME/.local/bin"
INSTALL_SHARE="$HOME/.local/share/$APP_NAME"
INSTALL_CONFIG="$HOME/.config/$APP_NAME"

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

rm -rf build

cmake -S . -B build

cmake --build build -j$(nproc)

ctest --test-dir build --output-on-failure

mkdir -p "$INSTALL_BIN"
mkdir -p "$INSTALL_SHARE"
mkdir -p "$INSTALL_CONFIG"

mkdir -p "$INSTALL_SHARE/data"
touch "$INSTALL_SHARE/data/players.bin"
touch "$INSTALL_SHARE/data/index.bin"
touch "$INSTALL_SHARE/data/clanes.bin"

cp build/taller_client "$INSTALL_SHARE/"
cp build/taller_server "$INSTALL_SHARE/"
cp -r assets "$INSTALL_SHARE/"
cp config.toml "$INSTALL_CONFIG/"
cp config.toml "$INSTALL_SHARE/"
cp config/client.toml "$INSTALL_CONFIG/client.toml"
cp config/client.toml "$INSTALL_SHARE/client.toml"
mkdir -p "$INSTALL_SHARE/config"
cp config/client.toml "$INSTALL_SHARE/config/client.toml"

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

mkdir -p "$HOME/.local/bin"

if ! grep -qxF 'export PATH="$HOME/.local/bin:$PATH"' "$HOME/.bashrc"; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
fi

if ! grep -qxF 'export PATH="$HOME/.local/bin:$PATH"' "$HOME/.profile"; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.profile"
fi

export PATH="$HOME/.local/bin:$PATH"
