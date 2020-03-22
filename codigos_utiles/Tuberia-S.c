/* 
Tuberia.c
Tuberias con nombre: programa para el servidor.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define FIFONAME "myfifo" // Nombre que se le va a dar al archivo al ser creado. No necesariamente definirlo de esta forma, también puede ser con un string

int main(void){
	int n,fd;
	char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.
	unlink(FIFONAME); // Elimina "myfifo" si existe.
	if(mkfifo(FIFONAME,0666)<0){ // Crea que archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if((fd=open(FIFONAME,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}
	while((n=read(fd,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos
		write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
	}
	close(fd);
	exit(0);
}
