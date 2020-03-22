/* 
 * Envio de mensajes por sockets. Cliente
 * El cliente se encarga si existe una conexión disponible para poder enviar mensajes en ip y puesto especificos
 * Similar a tuberias con nombre.
 * */

#include <stdio.h>
#include <unistd.h>
// Headers necesarios para establecer la conexión por sockets
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	int sockfd, n;
	char buf[1024];
	struct sockaddr_in servidor; // Estructura que almacenará donde y como se establecerá la conexión
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // Se crea el socket con el protocolo TCP/IP (AF_INET), destinado a envio de mensajes (SOCK_STREAM)
											  // El sistema operativo retorna el descriptor del socket.
	
	servidor.sin_family = AF_INET; // Se establece el protocolo de comunicación en la estructura
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1"); // Se establece la ip donde se aloja el socket
    servidor.sin_port = htons(8080); // Se establece el puerto donde se aloja el socket (Ver clase 6)
	// Tanto la ip y el puerto son definidos por el servidor. Por lo tanto, primero debe estar corriendo del servidor.
    if( connect(sockfd, (struct sockaddr *) &servidor, sizeof(servidor)) == -1 ) { // Se trata de establecer la conexión con la preconfiguración hecha
		printf("No se pudo establecer la conexión");
        return -1;
    }
    
    printf("Conexión establecida!\nEscribe sus mensajes...\n\n");
    
    while(1) {
		n = read(0, buf, 1024); // Cliente escribe mensaje
		if( send(sockfd, buf, n, 0) == -1 ) { // send es la función encargada de enviar mensajes por medio de sockets. Especificando el descriptor
											  // del socket (sockfd). Se envia la cadena (buf), y la dimensión del mensaje (n)
            printf("\nSe ha perdido la conexion\n");
            return -1;
        }
	}
    
	return 0;
	
}
