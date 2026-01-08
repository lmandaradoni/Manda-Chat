#Manda-Chat: Sistema de Chat Híbrido (C + Tauri/React)

## Descripción
Manda-Chat es una aplicación de mensajería cliente-servidor que combina el lenguaje C para la gestión de sockets y redes, y Tauri para la interfaz de usuario.

El objetivo de este proyecto fue aprender a integrar mi codigo en backend en C a una interfaz mediante el uso de pipes (stdin / stdout) y como conectar usuarios a traves de internet.

##Arquitectura del Sistema
El sistema se compone de tres partes fundamentales que se comunican entre sí:

1.  Core (C): Manejo de sockets TCP, hilos y sincronización.
2.  Bridge (Rust/Tauri): Este hace el manejo de las entradas/salidas del codigo.
3.  UI (React/TypeScript): Interfaz gráfica para el usuario.

Flujo de Comunicación:
UI (React)` <-> Tauri <-> Pipes (JSON) <-> Core (C) <-> Sockets TCP <-> Servidor

## Características Principales
- Arquitectura Multihilo: Uso de pthreads en C para manejar la escucha de mensajes y el envío de comandos simultáneamente sin bloquear la UI.
- Comunicación entre Procesos (IPC): Implementación de un protocolo basado en JSON a través de stdin y stdout para comunicar Rust con C.
- Sockets TCP: Implementación nativa de sockets en C (AF_INET y SOCK_STREAM).
- Manejo de Errores y Concurrencia: Control robusto de desconexiones, manejo de señales y prevención de condiciones de carrera en el backend.
- Interfaz Reactiva:** UI moderna construida con React y TypeScript.

## Tecnologías Utilizadas
- **Backend:** C (Sockets, Pthreads, Semaphore, Mutex).
- **Middleware:** Rust (Tauri framework, `std::process::Command`, `std::sync::mpsc`).
- **Frontend:** React, TypeScript, CSS.
- **Herramientas:** GCC, Make, NPM/Node.js.

## Instalación y Ejecución

### Prerrequisitos
-Compilador GCC y Make.
-Rust y Cargo instalados.
-Node.js y NPM.
-Librerias
-Etc.

-Este programa tiene varias dependencias, por lo que facilité un script con todo lo necesario para su instalación.
-Para ello ve a alguna terminal de Linux y haz:
	cd Manda-Chat
	./install.sh
	
-Este script tambien compila el codigo, de modo que podrás ejecutarlo inmediatamente despues

### Ejecución
    Haz en tu terminal de linux:
    
	cd Manda-Chat

#### Para levantar un servidor debes hacer:
	./run-server.sh

	Este script ejecutara el codigo y emitirá en la terminal una ip y puerto, los cuales deberas pasar a los usuarios que deseen ingresar a chatear.
	Para este codigo yo utilicé bore como herramienta para hacer un tunel TCP, el cual hará todo automaticamente.
	(dentro del codigo la ip es 127.0.0.1 y el puerto 9191)

#### Para conectarte a un servidor debes hacer:
	Primero el servidor debe estar en ejecución antes de intentar conectarte.
	Luego haz:

	cd Manda-Chat
	./run-client.sh

	(Esto tardará un poco en cargar si es la primera vez que ejecutas el codigo). 
	Si todo salió bien se abrirá una interfaz con la que podrás interactuar!!!
