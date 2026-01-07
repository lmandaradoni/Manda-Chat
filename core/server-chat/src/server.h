#ifndef SERVER_H
#define SERVER_H


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

typedef struct{
    char* nombre;
    int socket;
}t_user;



//HEADERS PARA SOCKETS:
int iniciar_servidor(char* puerto);

t_log* iniciar_logger(void);
t_config* iniciar_config(char* rutaConfig);

void* aceptar_clientes(void* arg);
void atender_cliente(void* arg);

void manejar_desconexion(int fd_conexion_ptr);

#endif /* SERVER_H */