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
#define FIFONAME "myfifo" // Nombre que se le va a dar al archivo al ser creado. No necesariamente definirlo de esta forma, también puede ser con un string

int main (void){
	int n,fd;
	char buf[1024]; // Cadena de char usado para guardar lo que se escribe en el cliente.
	if((fd=open(FIFONAME,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	while((n=read(0,buf,sizeof(buf)))>0){ // Lee desde teclado el mensaje que se quiere enviar al servidor.
		write(fd,buf,n); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
	}
	close(fd);
	exit(0);
}
