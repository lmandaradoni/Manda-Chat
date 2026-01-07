#include "server.h"

t_log* logger;
t_config* config;

t_list* usuarios_conectados;

int main() {
    signal(SIGPIPE, SIG_IGN);
    logger = iniciar_logger();

    char* puerto = "9191";
    char* ip        = "127.0.0.1";
    
    
    sem_t sem_el_inmortal;
    sem_init(&sem_el_inmortal, 0, 0);
    
    usuarios_conectados = list_create();

    int socket_server = iniciar_servidor(puerto);
    log_info(logger, "Esperando clientes en el puerto %s : %s", puerto, ip);



    
    pthread_t hilo_aceptar_clientes;      
    pthread_create(&hilo_aceptar_clientes, NULL, (void*) aceptar_clientes, &socket_server);
    pthread_detach(hilo_aceptar_clientes);





    sem_wait(&sem_el_inmortal);
    return 0;
}