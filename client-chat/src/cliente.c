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

    log_info(logger, "se creo la conexion al servidor %s : %s", ip, puerto);
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



void enviar_mensajes(const char* nombre, int socket){
    
    while(1){
        char mensaje[512];
        char mensaje_final[1024];
        const char* nombre_usuario = nombre;
        //printf("Ingrese su mensaje: \n");
        fflush(stdout);
        
        //desactivar_echo();
        fgets(mensaje, sizeof(mensaje), stdin);
        //activar_echo();
        printf("\033[1A\033[2K\r"); 
        fflush(stdout);

        mensaje[strcspn(mensaje, "\n")] = '\0';
        snprintf(mensaje_final, sizeof(mensaje_final),
                "%s: %s", nombre_usuario, mensaje);


        //log_info(logger, "Enviando mensaje: %s", mensaje_final);

        int offset = 0;
        int largo_mensaje = strlen(mensaje_final) + 1;
        int tam_buffer = sizeof(int) + largo_mensaje;
        void *buffer = malloc(tam_buffer);
        memcpy(buffer + offset, &largo_mensaje, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, mensaje_final, largo_mensaje);
        offset += largo_mensaje;

        
        send(socket, &tam_buffer, sizeof(int), 0);
        send(socket, buffer, tam_buffer, 0);
        
        printf("\033[2K\r");
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

        if (recv(socket, &tam_buffer, sizeof(int), MSG_WAITALL) <= 0) {
            log_info(logger, "No hay mensajes nuevos del servidor");
            break;              
        }

        void* buffer = malloc(tam_buffer);
        if (recv(socket, buffer, tam_buffer, MSG_WAITALL) <= 0) {
            log_error(logger, "Error recibiendo buffer del servidor");
            free(buffer);
            close(socket);
            break;
        }

        //log_info(logger, "Nuevo mensaje recibido del servidor:");
        memcpy(&tam_mensaje, buffer + offset, sizeof(int));
        offset += sizeof(int);
        //log_info(logger, "Tamaño del mensaje: %d", tam_mensaje);
        mensaje = malloc(tam_mensaje);
        memcpy(mensaje, buffer + offset, tam_mensaje);
        //log_info(logger, "Mensaje recibido: %s", mensaje);
        offset += tam_mensaje;

    
        log_info(logger, "%s", mensaje);


        free(mensaje);
        free(buffer);
    }

    return NULL;
}


void desactivar_echo() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void activar_echo() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}