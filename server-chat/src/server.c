#include "server.h"


int iniciar_servidor(char *puerto) {
    struct addrinfo hints, *servinfo;
    int socket_escucha, err;
    int opt = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if ((err = getaddrinfo(NULL, puerto, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }

    socket_escucha = socket(servinfo->ai_family,
                            servinfo->ai_socktype,
                            servinfo->ai_protocol);
    if (socket_escucha == -1) {
        perror("socket");
        freeaddrinfo(servinfo);
        return -1;
    }

    if (setsockopt(socket_escucha, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(socket_escucha);
        freeaddrinfo(servinfo);
        return -1;
    }

    if (bind(socket_escucha, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        perror("bind");
        close(socket_escucha);
        freeaddrinfo(servinfo);
        return -1;
    }

    if (listen(socket_escucha, SOMAXCONN) == -1) {
        perror("listen");
        close(socket_escucha);
        freeaddrinfo(servinfo);
        return -1;
    }

    freeaddrinfo(servinfo);
    
    return socket_escucha;
}



int esperar_clientes(int socket_servidor){

    int socket_cliente = accept(socket_servidor, NULL, NULL);

    //printf("Se conecto un cliente!!\n");

    return socket_cliente;
}

t_log* iniciar_logger(void){   
    t_log* nuevo_logger= log_create("master.log", "Master Logger", 1, LOG_LEVEL_INFO);
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



