#include <stdio.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define TRUE  1
#define FALSE 0
#define PUERTO 8080
#define NUM_CLIENTES 2

int main() {
	int sockfd, connfd, lenbuf, i;
	int clientes_socket[2] = {0,0};      // Lista de clientes/jugadores
	socklen_t lencli;
	char buf[1024];
	struct sockaddr_in servidor; // Estructura que almacenará donde y como se establecerá la conexión.
	struct sockaddr cliente; // Estructura donde se almacena la información de conexión de los clientes.
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // Se crea el socket con el protocolo TCP/IP (AF_INET), destinado a envio de mensajes (SOCK_STREAM)
											  // El sistema operativo retorna el descriptor del socket.

	if(sockfd == -1) {
        return -1;
    }
    
    servidor.sin_family = AF_INET; // Se establece el protocolo de comunicación en la estructura
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); // El servidor define la ip donde alojará el socket
    servidor.sin_port = htons(PUERTO); // También el puerto
    
    if( bind( sockfd, (struct sockaddr*) &servidor, sizeof(servidor) ) != 0 ) { // Inicializa el socket para el uso local (eventualmente también se puede remotamente)
        return -1;
    }
    
    if( listen(sockfd, NUM_CLIENTES) != 0 ) { // Establece cuantos usuarios se van a conectar, en este caso son 2 (jugadores).
        return -1;
    }
    printf("Esperando jugadores...\n");
	
	
	while(TRUE) { // Se inicia el módulo para controlar conexiones y acciones del juego

		for(i=0; i<NUM_CLIENTES; i++) {

			connfd = accept(sockfd, &cliente, &lencli); // Espera hasta que se conecte un cliente y se almacena su información

            if(connfd < 0) {
		        return -1;
            }

            // Agrega el fd del nuevo socket a la lista de clientes/jugadores
            if(clientes_socket[i] == 0) {   

                clientes_socket[i] = connfd;
                printf("Agregando jugador numero: %d\n" , i+1); 
            }

            // Algo con fork()...

            if (i==0) { // Si aun no llega a conectar 2 jugadores
            	printf("Jugador conectado!\nFalta 1 jugador mas para iniciar, esperando jugador...\n\n");
            } else {
            	printf("Jugador conectado!\nIniciando partida, cargando...\n");
            	break;
            }
		}

		if( (lenbuf = recv(connfd, buf, 1024, 0)) == 0 ) { // recv se encarga de recibir mensajes desde algún cliente, almacenando de una cadena de caracteres
			printf("\nError al recibir mensaje\n");
			return -1;
		} else {
			write(1, buf, lenbuf); // Lo escribe por pantalla
		}
	}
	
	return 0;
}
