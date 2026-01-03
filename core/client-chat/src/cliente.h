#include <commons/log.h>
#include <commons/config.h>
//#include <utils/hello.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <signal.h>

#include <termios.h>
#include <unistd.h>

#define EVT_CONNECTED   "conexion"
#define EVT_MESSAGE     "mensaje"
#define EVT_ERROR       "error"

#define CMD_SEND        "send"
#define CMD_QUIT        "quit"

int crear_conexion(char *ip, char *puerto);

t_log* iniciar_logger(void);
t_config* iniciar_config(char* rutaConfig);

void enviar_mensajes(const char* nombre, int socket);

void* escuchar_mensajes(void* arg);


void emit_conexion(const char* server, const char* port);
void emit_mensaje(const char* from, const char* text);
void emit_error(const char* msg);

void loop_comandos(int socket, const char* nombre);
void enviar_mensaje(const char* nombre, int socket, const char* mensaje);


void esperar_init(char* nombre, char* ip, char* puerto);
void extraer_string(const char* json, const char* key, char* destino);
