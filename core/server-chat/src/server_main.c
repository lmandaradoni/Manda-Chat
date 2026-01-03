#include "server.h"

t_log* logger;
t_config* config;

t_list* usuarios_conectados;

int main() {

    logger = iniciar_logger();

    //log_info(logger, "path es /home/luca/Manda-Chat/server-chat/src/server.config");
    
    //config = iniciar_config("/home/luca/Manda-Chat/server-chat/src/server.config");
    // char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    // char* ip        = config_get_string_value(config, "IP_SERVER");

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