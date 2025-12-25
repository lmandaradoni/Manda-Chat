#include "cliente.h"

t_log* logger;
t_config* config;

int main() {

    logger = iniciar_logger();    
    config = iniciar_config("/home/utnso/Manda-Chat/client-chat/src/cliente.config");

    char* puerto    = config_get_string_value(config, "PUERTO_SERVER");
    char* ip        = config_get_string_value(config, "IP_SERVER");


    int socket_server = crear_conexion(ip, puerto);

    log_info(logger, "se creo la conexion al servidor %s : %s", ip, puerto);


    printf("CLIENTE: HOLA\n");


    

    return 0;
} 