#include "server.h"

extern t_log* logger;
extern t_config* config;
extern t_list* usuarios_conectados;
    
pthread_mutex_t mutex_conectados;

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



void* aceptar_clientes(void* arg){
    int servidor_escucha = *(int*)arg;
    //free(arg);

    log_debug(logger, "Listo para aceptar QUERYS y WORKERS");

    pthread_mutex_init(&mutex_conectados, NULL);

    
    while(1){
        int* fd_conexion_ptr = malloc(sizeof(int));
        *fd_conexion_ptr = esperar_clientes(servidor_escucha);
        //log_info(logger, "Se conecto un cliente!!");
        

        //HANDSHAKE
        int tam_buffer;
        int offset = 0;
        int tamanio_nombre_user;
        char* nombre_user;

        recv(*fd_conexion_ptr, &tam_buffer, sizeof(int), MSG_WAITALL);
        //log_info(logger, "Tam buffer recibido: %d", tam_buffer);

        char* buffer = malloc(tam_buffer);
        if (recv(*fd_conexion_ptr, buffer, tam_buffer, MSG_WAITALL) <= 0) {
            log_error(logger, "CLIENTE DESCONECTADO");
            free(buffer);
            
            continue;
        }

        //log_info(logger, "Recibo buffer");

        memcpy(&tamanio_nombre_user, buffer + offset, sizeof(int));
        offset += sizeof(int);

        if (tamanio_nombre_user <= 0 || tamanio_nombre_user > 1024) {
            log_error(logger, "Tamaño de nombre inválido: %d", tamanio_nombre_user);
            free(buffer);
            continue;
        }

        
        // nombre_user = malloc(tamanio_nombre_user);
        // memcpy(nombre_user, buffer + offset, tamanio_nombre_user);

        nombre_user = malloc(tamanio_nombre_user + 1);
        memcpy(nombre_user, buffer + offset, tamanio_nombre_user);

        offset += tamanio_nombre_user;

        log_info(logger, "Bienvenido: %s", nombre_user);


        t_user* usuario = malloc(sizeof(t_user));
        usuario->nombre = nombre_user;
        usuario->socket = *fd_conexion_ptr;

        //log_info(logger, "Agregando usuario a la lista de conectados");

        pthread_mutex_lock(&mutex_conectados);
        list_add(usuarios_conectados, usuario);
        pthread_mutex_unlock(&mutex_conectados);

        //log_info(logger, "Usuario %s agregado a la lista de conectados", nombre_user);

        


        pthread_t hilo_atender_query;
        pthread_create(&hilo_atender_query, NULL, (void*) atender_cliente, fd_conexion_ptr);
        pthread_detach(hilo_atender_query);
        
        free(nombre_user);
        free(buffer);
    
    }
}


void atender_cliente(void* arg){
    int fd_conexion_ptr = *(int*)arg;    
    free(arg);


    while(1){

        //ACA RECIBIMOS LOS MENSAJES DE LOS  CLIENTES 
        //Y LOS REENVIAMOS A TODOS LOS DEMAS
        int offset = 0;
        int tam_buffer;
        char* mensaje;
        int tam_mensaje;

        int tam_nombre;
        char* nombre;
        //log_info(logger, "Esperando mensajes de los clientes...");

        if (recv(fd_conexion_ptr, &tam_buffer, sizeof(int), MSG_WAITALL) <= 0) {
            log_info(logger, "Error: posible desconexion %d", fd_conexion_ptr);
            
            continue;              
        }

        void* buffer = malloc(tam_buffer);
        if (recv(fd_conexion_ptr, buffer, tam_buffer, MSG_WAITALL) <= 0) {
            log_error(logger, "Error recibiendo buffer del cliente %d", fd_conexion_ptr);

            free(buffer);
            continue;
        }

        
        
        
        //log_info(logger, "Hay %d usuarios conectados", list_size(usuarios_conectados));
        
        pthread_mutex_lock(&mutex_conectados);
        for(int i =0; i<list_size(usuarios_conectados); i++){
            t_user* usuario = list_get(usuarios_conectados, i);
            int socket_cliente = usuario->socket;
            //log_info(logger, "Enviando mensaje al usuario %s en el socket %d", usuario->nombre, socket_cliente);
            
            send(socket_cliente, &tam_buffer, sizeof(int), 0);
            send(socket_cliente, buffer, tam_buffer, 0);
            
        }
        pthread_mutex_unlock(&mutex_conectados);
        

        memcpy(&tam_nombre, buffer + offset, sizeof(int));
        offset += sizeof(int);
        nombre = malloc(tam_nombre);
        memcpy(nombre, buffer + offset, tam_nombre);
        offset += tam_nombre;
        memcpy(&tam_mensaje, buffer + offset, sizeof(int));
        offset += sizeof(int);
        mensaje = malloc(tam_mensaje);
        memcpy(mensaje, buffer + offset, tam_mensaje);
        offset += tam_mensaje;

        log_info(logger, "%s: %s",nombre, mensaje);

        free(mensaje);
        free(buffer);
        free(nombre);
    }

}