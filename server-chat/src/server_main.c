#include "server.h"

t_log* logger;
t_config* config;

int main() {

    logger = iniciar_logger();
    config = iniciar_config("/home/utnso/Manda-Chat/server-chat/src/server.config");
    
    char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    char* ip        = config_get_string_value(config, "IP_SERVER");

    
    int socket_server = iniciar_servidor(puerto);
    log_info(logger, "Esperando clientes en el puerto %s : %s", puerto, ip);
    int fd_escucha = esperar_clientes(socket_server);
    log_info(logger, "Se conecto un cliente!!");

    return 0;
}