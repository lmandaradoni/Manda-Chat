#include "cliente.h"


int crear_conexion(char *ip, char *puerto){
    struct addrinfo hints, *server_info;
    int socket_cliente;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(ip, puerto, &hints, &server_info);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    socket_cliente = socket(server_info->ai_family,
                            server_info->ai_socktype,
                            server_info->ai_protocol);
    if (socket_cliente == -1) {
        perror("socket");
        freeaddrinfo(server_info);
        return -1;
    }

    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        perror("connect");
        close(socket_cliente);
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);
    return socket_cliente;
}



t_log* iniciar_logger(void){   
    t_log* nuevo_logger= log_create("cliente.log", "Cliente Logger", 1, LOG_LEVEL_INFO);
    if(nuevo_logger == NULL)
    {
        perror("No se pudo crear el log \n");
        exit(EXIT_FAILURE);
    }
    return nuevo_logger;
}                                   
t_config* iniciar_config(char* rutaConfig){
    t_config* nuevo_config = config_create(rutaConfig);

    if(nuevo_config==NULL){
        perror("Error al cargar el config \n");
        exit(EXIT_FAILURE);
    }
    
    return nuevo_config;  
}



