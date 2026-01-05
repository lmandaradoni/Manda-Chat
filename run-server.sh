#!/usr/bin/env bash

# --- CONFIGURACIÓN DE RUTAS ---
PUERTO_LOCAL=9191
RAIZ_PROYECTO=$(pwd)

SERVER_DIR="$RAIZ_PROYECTO/core/server-chat"
# Si no está en core, intenta en la raíz directamente
if [ ! -d "$SERVER_DIR" ]; then
    SERVER_DIR="$RAIZ_PROYECTO/server-chat"
fi

BIN_SERVER="$SERVER_DIR/bin/server-chat"

if command -v bore >/dev/null 2>&1; then
    CMD_BORE="bore"
elif [ -f "$RAIZ_PROYECTO/bore" ]; then
    CMD_BORE="$RAIZ_PROYECTO/bore"
else
    echo "Error: No se encuentra 'bore'. Instálalo con 'cargo install bore-cli' o descarga el binario a la raíz."
    exit 1
fi

if [ ! -d "$SERVER_DIR" ]; then
    echo "Error: No se encontró la carpeta 'server-chat' en core/ ni en la raíz."
    exit 1
fi

if [ ! -f "$BIN_SERVER" ]; then
    echo "🛠️  Compilando servidor..."
    cd "$SERVER_DIR" && make && cd "$RAIZ_PROYECTO"
fi

cleanup() {
    echo -e "\n\n🛑 Cerrando todo..."
    kill $SERVER_PID 2>/dev/null
    kill $BORE_PID 2>/dev/null
    exit
}
trap cleanup SIGINT

echo "Iniciando Servidor en $SERVER_DIR"
cd "$SERVER_DIR"
./bin/server-chat > "$RAIZ_PROYECTO/server.log" 2>&1 &
SERVER_PID=$!

echo "Iniciando Túnel Bore..."
$CMD_BORE local $PUERTO_LOCAL --to bore.pub &
BORE_PID=$!

echo "--------------------------------------------------------"
echo "Servidor corriendo. Logs en: server.log"
echo "Espera a que Bore te asigne un puerto arriba."
echo "Presiona Ctrl+C para detener."
echo "--------------------------------------------------------"

wait $BORE_PID