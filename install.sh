#!/usr/bin/env bash

#!/bin/bash

#!/bin/bash
set -e

echo "Instalando Manda-Chat..."

# 1. debe ser solo WSL / Linux
if ! command -v apt >/dev/null; then
  echo "Este instalador es solo para Linux / WSL"
  exit 1
fi

# 2. Dependencias
echo "Instalando dependencias..."
sudo apt update
sudo apt install -y \
  build-essential \
  git \
  libreadline-dev

# 3. Commons
if [ ! -d "so-commons-library" ]; then
  echo "Clonando librerias necesarias..."
  git clone https://github.com/sisoputnfrba/so-commons-library.git
fi

cd so-commons-library
make
sudo make install
cd ..

# 4. Compilar proyecto
echo "Compilando..."
make

# 5. Permisos
chmod +x server-chat/bin/server-chat
chmod +x client-chat/bin/client-chat

echo "Instalación completa"
