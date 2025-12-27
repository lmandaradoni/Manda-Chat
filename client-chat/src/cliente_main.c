#include "cliente.h"

t_log* logger;
t_config* config;

int main() {

    logger = iniciar_logger();    
    config = iniciar_config("/home/utnso/Manda-Chat/client-chat/src/cliente.config");

    char* puerto    = config_get_string_value(config, "PUERTO_SERVER");
    char* ip        = config_get_string_value(config, "IP_SERVER");


    char nombre[50];

    printf("Ingrese su nombre: ");
    fflush(stdout);   // importante si usás logs / buffers

    fgets(nombre, sizeof(nombre), stdin);

    // eliminar el '\n' que agrega fgets
    nombre[strcspn(nombre, "\n")] = '\0';

    printf("Hola %s!\n", nombre);

    int socket_server = crear_conexion(ip, puerto);

    
    //enviarmos el nombre al servidor
    int offset = 0;
    int largo_nombre = strlen(nombre) + 1;
    int tam_buffer = sizeof(int) + largo_nombre;
    void *buffer = malloc(tam_buffer);

    memcpy(buffer + offset, &largo_nombre, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, nombre, largo_nombre);
    offset += largo_nombre;
    
    //log_info(logger, "enviando nombre al servidor: %s", nombre);
    //log_info(logger, "tam buffer: %d", tam_buffer);
    send(socket_server, &tam_buffer, sizeof(int), 0);
    send(socket_server, buffer, tam_buffer, 0);
    free(buffer);


    //TODO HILO ESCUCHAR MENSAJES DE OTROS

    pthread_t hilo_escuchar_mensajes;      
    pthread_create(&hilo_escuchar_mensajes, NULL, (void*) escuchar_mensajes, &socket_server);
    pthread_detach(hilo_escuchar_mensajes);

    enviar_mensajes(nombre, socket_server);

    return 0;
} 