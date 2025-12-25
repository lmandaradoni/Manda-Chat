#include "server.h"

t_log* logger;
t_config* config;

int main() {

    logger = iniciar_logger();
    config = iniciar_config("/home/luca/Manda-Chat/server-chat/src/server.config");
    log_info(logger, "LOG SERVER CREADO");
    
    printf("SERVER: HOlis \n");


    
    char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");;
    log_info(logger, "puerto del servidor %s", puerto);
    
    iniciar_servidor(puerto);
    
    return 0;
}