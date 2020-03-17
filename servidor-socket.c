#include <stdio.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main() {
	int sockfd, connfd, lenbuf;
	socklen_t lencli;
	char buf[1024];
	struct sockaddr_in servidor; // Estructura que almacenará donde y como se establecerá la conexión
	struct sockaddr cliente; // Estructura donde se almacena la información de conexión del cliente.
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // Se crea el socket con el protocolo TCP/IP (AF_INET), destinado a envio de mensajes (SOCK_STREAM)
											  // El sistema operativo retorna el descriptor del socket.
	
	if(sockfd == -1) {
        return -1;
    }
    
    servidor.sin_family = AF_INET; // Se establece el protocolo de comunicación en la estructura
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); // El servidor define la ip donde alojará el socket
    servidor.sin_port = htons(8080); // También el puerto
    
    if( bind( sockfd, (struct sockaddr*) &servidor, sizeof(servidor) ) != 0 ) { // Inicializa el socket para el uso local (eventualmente también se puede remotamente)
        return -1;
    }
    
    if( listen(sockfd, 1) != 0 ) { // Establece cuandos usuarios se van a conectar, en este caso se establece solo uno.
        return -1;
    }
    printf("Esperando cliente...\n");
    connfd = accept(sockfd, &cliente, &lencli); // Espera hasta que se conecte un cliente y se almancena su información
    
    if(connfd < 0) {
		return -1;
	}
	
	printf("Cliente conectado!\nAhora se puede recibir mensajes del cliente...\n\n");
	
	while(1) {
		if( (lenbuf = recv(connfd, buf, 1024, 0)) == 0 ) { // recv se encarga de recibir mensajes desde algún cliente, almacenando de una cadena de caracteres
			printf("\nError al recibir mensaje\n");
			return -1;
		}else {
			write(1, buf, lenbuf); // Lo escribe por pantalla
		}
	}
	
	return 0;
}
