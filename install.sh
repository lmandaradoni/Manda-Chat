#!/usr/bin/env bash
set -e

echo "Instalando dependencias de sistema..."
sudo apt update
sudo apt install -y build-essential git libreadline-dev libssl-dev libgtk-3-dev libwebkit2gtk-4.1-dev pkg-config curl

# --- RUST (Requerido para Tauri y para instalar Bore) ---
if ! command -v cargo &> /dev/null; then
    echo "Instalando Rust..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
else
    echo "Rust ya está instalado."
fi

# --- BORE (Túnel TCP) ---
if ! command -v bore &> /dev/null; then
    echo "Instalando Bore (Túnel TCP)... esto puede demorar unos minutos."
    # Instalamos bore de forma global en el sistema a través de cargo
    cargo install bore-cli
else
    echo "Bore ya está instalado."
fi

# --- SO COMMONS ---
if [ ! -d "so-commons-library" ]; then
    echo "Instalando so-commons..."
    git clone https://github.com/sisoputnfrba/so-commons-library.git
fi
cd so-commons-library && make && sudo make install && cd ..

# --- COMPILAR C ---
echo "Compilando Cliente y Servidor..."
# Corregimos rutas basándonos en tu estructura
if [ -d "core/client-chat" ]; then
    cd core/client-chat && make && cd ../..
fi

if [ -d "server-chat" ]; then
    cd server-chat && make && cd ..
elif [ -d "core/server-chat" ]; then
    cd core/server-chat && make && cd ../..
fi

# --- NODE DEPENDENCIES ---
echo "Instalando dependencias de la UI..."
if [ -d "ui/manda-chat-ui" ]; then
    cd ui/manda-chat-ui && npm install && cd ../..
fi

echo "----------------------------------------"
echo "Instalación completa."
echo "IMPORTANTE: Si es la primera vez que instalas Rust,"
echo "ejecuta 'source \$HOME/.cargo/env' antes de correr el server."
echo ""
echo "EJECUCION:"
echo "Para ejecutar el server: ./run-server.sh"
echo "Para ejecutar un cliente (usuarios que se conecten al server): ./run-client.sh"
echo "----------------------------------------"