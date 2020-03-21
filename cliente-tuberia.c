/* 
Tuberia.c
Tuberias con nombre: programa para el cliente.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define FIFOC1 "myfifo-cliente-1" // Nombre del archivo para enlace cliente 1 (es un nombre de referencia únicamente)
#define FIFOC2 "myfifo-cliente-2" // ** cliente 2

int main (void){
	int n,fd;
	char buf[1024]; // Cadena de char usado para guardar lo que se escribe en el cliente.
	
	printf("Conectando en jugador 1...\n");
	if((fd=open(FIFOC1,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		
		perror("open"); // Si falla, error

		printf("Conectando en jugador 2...\n");
		if((fd=open(FIFOC2,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
			
			perror("open"); // Si falla, error
			exit(1);
		} else {
			printf("Conexion lograda - Preparando...\n");
			write(fd, "aqui-cliente", 12);
		}
	} else {
		printf("Conexion lograda - Preparando...\n");
		write(fd, "aqui-cliente", 12);
	}
    //chmod(FIFOC1, 0000);

    while((n = read(fd,buf,sizeof(buf)))>0) { // Lee mensajes desde el servidor, lo almacena en buf y retorna en número de bytes escritos

		if(buf[0] == '$') { // Si es su turno (definimos el $ para indicar turno)
			
			printf("Es su turno!, ingrese su eleccion --------\n")
			// Acciones para mostrar matriz juego (substring? o identificar quizá)
			n = read(0,buf,sizeof(buf)); // Lee desde teclado el mensaje que se quiere enviar al servidor.
            write(fd,buf,n); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes

		} else { // Se envia la matriz de juego

			write(1,buf,n); // Escribe por pantalla la matriz de juego.
		}
	}

	close(fd);
	exit(0);
}
