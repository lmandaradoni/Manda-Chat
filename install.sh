#!/usr/bin/env bash
set -e

echo "Instalando dependencias de sistema..."
sudo apt update
sudo apt install -y build-essential git libreadline-dev libssl-dev libgtk-3-dev libwebkit2gtk-4.1-dev pkg-config

# --- SO COMMONS ---
if [ ! -d "so-commons-library" ]; then
  git clone https://github.com/sisoputnfrba/so-commons-library.git
fi
cd so-commons-library && make && sudo make install && cd ..

# --- COMPILAR C ---
echo "Compilando Cliente y Servidor..."
cd core/client-chat && make && cd ../..
cd server-chat && make && cd ..

# --- NODE DEPENDENCIES ---
echo "Instalando dependencias de la UI..."
cd ui/manda-chat-ui && npm install && cd ../..

echo "Instalación completa."