#include "cliente.h"

extern t_log* logger;
extern t_config* config;    

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
        emit_error("crear server");
        freeaddrinfo(server_info);
        return -1;
    }

    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        emit_error("error al hacer connect");
        close(socket_cliente);
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);

    return socket_cliente;
}



t_log* iniciar_logger(void){   
    t_log* nuevo_logger= log_create("cliente.log", "Cliente Logger", 0, LOG_LEVEL_INFO);
    if(nuevo_logger == NULL)
    {
        perror("No se pudo crear el log \n");
        emit_error("error iniciar logger");
        exit(EXIT_FAILURE);
    }
    return nuevo_logger;
}                                   
t_config* iniciar_config(char* rutaConfig){
    t_config* nuevo_config = config_create(rutaConfig);

    if(nuevo_config==NULL){
        perror("Error al cargar el config \n");
        emit_error("error con config");
        exit(EXIT_FAILURE);
    }
    
    return nuevo_config;  
}



void enviar_mensajes(const char* nombre, int socket){
    
    while(1){
        char mensaje[1024];

        fflush(stdout);
        
        if(fgets(mensaje, sizeof(mensaje), stdin) == NULL){
            printf("STDIN cerrado\n");
            fflush(stdout);
            break;
        }

    
        printf("MENSAJE DESDE UI: %s", mensaje);
        fflush(stdout);
        
        int offset = 0;
        int largo_mensaje = strlen(mensaje) + 1;
        int largo_nombre = strlen(nombre) + 1;
        
        int tam_buffer = sizeof(int) * 2 + largo_nombre + largo_mensaje;

        
        void *buffer = malloc(tam_buffer);
        
        memcpy(buffer + offset , &largo_nombre, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, nombre, largo_nombre);
        offset += largo_nombre;
        memcpy(buffer + offset, &largo_mensaje, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, mensaje, largo_mensaje);
        offset += largo_mensaje;
        
        
        send(socket, &tam_buffer, sizeof(int), 0);
        send(socket, buffer, tam_buffer, 0);
        
        fflush(stdout);
        free(buffer);

    }
}




void* escuchar_mensajes(void* arg){
    int socket = *(int*)arg;

    while(1){
        int offset = 0;
        int tam_buffer;
        char* mensaje;
        int tam_mensaje;

        int tam_nombre;
        char* nombre;

        if (recv(socket, &tam_buffer, sizeof(int), MSG_WAITALL) <= 0) {
            printf("{\"type\":\"desconexion\",\"motivo\":\"servidor_caido\"}\n");
            fflush(stdout);
            close(socket);
            exit(1);             
        }

        void* buffer = malloc(tam_buffer);
        if (recv(socket, buffer, tam_buffer, MSG_WAITALL) <= 0) {

            printf("{\"type\":\"desconexion\",\"motivo\":\"servidor_caido\"}\n");
            fflush(stdout);
            close(socket);
            exit(1);
        }

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

        emit_mensaje(nombre, mensaje);

        free(mensaje);
        free(buffer);
        free(nombre);

    }

    return NULL;
}


void emit_conexion(const char* server, const char* port) {
    printf(
        "{\"type\":\"conexion\",\"server\":\"%s\",\"port\":%s}\n",
        server, port
    );
    fflush(stdout);
}

void emit_mensaje(const char* from, const char* text) {
    char limpio[1024];
    strncpy(limpio, text, sizeof(limpio));
    
    for(int i = 0; limpio[i]; i++) {
        if(limpio[i] == '\n' || limpio[i] == '\r') limpio[i] = ' ';
        if(limpio[i] == '"') limpio[i] = '\''; 
    }

    printf("{\"type\":\"mensaje\",\"de\":\"%s\",\"text\":\"%s\"}\n", from, limpio);
    fflush(stdout);
}

void emit_error(const char* msg) {
    printf(
        "{\"type\":\"error\",\"mensaje\":\"%s\"}\n",
        msg
    );
    fflush(stdout);
}


void loop_comandos(int socket, const char* nombre) {
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), stdin)) {
        log_info(logger, "el buffer contiene %s", buffer);
        

        if (strstr(buffer, "\"cmd\":\"send\"")) {

            char* inicio = strstr(buffer, "\"text\":\"");
            if (!inicio) continue;

            inicio += strlen("\"text\":\"");
            char* fin = strchr(inicio, '"');
            if (!fin) continue;

            *fin = '\0';

            enviar_mensaje(nombre, socket, inicio);
        }

        

        else if (strstr(buffer, "\"cmd\":\"quit\"")) {
            printf("{\"type\":\"desconexion\",\"motivo\":\"usuario\"}\n");
            fflush(stdout);
            break;
        }
        
        fflush(stdout);
    }
    close(socket);
}


void enviar_mensaje(const char* nombre, int socket, const char* mensaje) {

    int offset = 0;
    int largo_mensaje = strlen(mensaje) + 1;
    int largo_nombre = strlen(nombre) + 1;

    int tam_buffer = sizeof(int) * 2 + largo_nombre + largo_mensaje;
    void *buffer = malloc(tam_buffer);

    memcpy(buffer + offset, &largo_nombre, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, nombre, largo_nombre);
    offset += largo_nombre;

    memcpy(buffer + offset, &largo_mensaje, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, mensaje, largo_mensaje);

    send(socket, &tam_buffer, sizeof(int), 0);
    send(socket, buffer, tam_buffer, 0);

    free(buffer);
}


void esperar_init(char* nombre, char* ip, char* puerto) {
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), stdin)) {

        if (strstr(buffer, "\"cmd\":\"init\"")) {

            extraer_string(buffer, "name", nombre);
            extraer_string(buffer, "ip", ip);
            extraer_string(buffer, "port", puerto);

            return;
        }
    }
}

void extraer_string(const char* json, const char* key, char* destino) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\":\"", key);

    char* start = strstr(json, pattern);
    if (!start) {
        destino[0] = '\0';
        return;
    }

    start += strlen(pattern);
    char* end = strchr(start, '"');
    if (!end) {
        destino[0] = '\0';
        return;
    }

    size_t len = end - start;
    strncpy(destino, start, len);
    destino[len] = '\0';
}



