#include "cliente.h"

t_log* logger;
t_config* config;

int main() {

    logger = iniciar_logger();    
    //log_info(logger, "path es /home/luca/Manda-Chat/client-chat/src/cliente.config");
    //config = iniciar_config("/home/luca/Manda-Chat/client-chat/src/cliente.config");

    //char* puerto    = config_get_string_value(config, "PUERTO_SERVER");
    //char* ip        = config_get_string_value(config, "IP_SERVER");
    
    setbuf(stdout, NULL);
    
    char nombre[50];
    char puerto[20];
    char ip[20];
    
    esperar_init(nombre, ip, puerto);

    int socket_server = crear_conexion(ip, puerto);


    if (socket_server == -1) {

        return 1; 
    }
    
    emit_conexion(ip, puerto);
    
    
    //enviamos el nombre al servidor
    int offset = 0;
    int largo_nombre = strlen(nombre) + 1;
    int tam_buffer = sizeof(int) + largo_nombre;
    void *buffer = malloc(tam_buffer);

    if (buffer == NULL) {
        emit_error("Error de memoria");
        close(socket_server);
        return 1;
    }
    
    memcpy(buffer + offset, &largo_nombre, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, nombre, largo_nombre);
    offset += largo_nombre;
    
    //log_info(logger, "enviando nombre al servidor: %s", nombre);
    //log_info(logger, "tam buffer: %d", tam_buffer);
    if(send(socket_server, &tam_buffer, sizeof(int), 0) == -1){
        emit_error("Error enviando handshake");
        free(buffer);
        close(socket_server);
        return 1;
    }
    if(send(socket_server, buffer, tam_buffer, 0) == -1){
        emit_error("Error enviando handshake");
        free(buffer);
        close(socket_server);
        return 1;
    }
    free(buffer);
    
    
    //TODO HILO ESCUCHAR MENSAJES DE OTROS
    
    pthread_t hilo_escuchar_mensajes;      
    pthread_create(&hilo_escuchar_mensajes, NULL, (void*) escuchar_mensajes, &socket_server);
    pthread_detach(hilo_escuchar_mensajes);
    
    loop_comandos(socket_server, nombre);

    //enviar_mensajes(nombre, socket_server);

    return 0;
} 
