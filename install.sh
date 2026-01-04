#!/usr/bin/env bash
set -e

echo "Instalando dependencias de sistema para Tauri y C..."

sudo apt update
sudo apt install -y \
  build-essential \
  curl \
  wget \
  libssl-dev \
  libgtk-3-dev \
  libayatana-appindicator3-dev \
  librsvg2-dev \
  libwebkit2gtk-4.1-dev \
  libreadline-dev \
  git \
  pkg-config

# --- Instalar Rust si no está ---
if ! command -v cargo >/dev/null; then
  echo "Instalando Rust..."
  curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
  source $HOME/.cargo/env
fi

# --- Instalar Node.js si no está (opcional, pero recomendado) ---
# Aquí podrías usar NVM o simplemente avisar que lo instalen.

# --- SO COMMONS (Igual que antes) ---
if [ ! -d "so-commons-library" ]; then
  git clone https://github.com/sisoputnfrba/so-commons-library.git
fi
cd so-commons-library && make && sudo make install && cd ..

# --- Compilar C (Igual que antes) ---
cd server-chat && make && cd ..
cd client-chat && make && cd ..

# --- Instalar dependencias de NPM ---
npm install

echo "¡Todo listo! Ahora puedes ejecutar: npm run tauri dev"