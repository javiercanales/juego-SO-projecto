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

/**
* En este sistema se crean 3 tuberias, la primera, creada por el padre (con mkfifo(FIFONAME,0666))
* La segunda y tercera, creadas por
*/

#define FIFONAME "myfifo" // Nombre que se le va a dar al archivo al ser creado. No necesariamente definirlo de esta forma, también puede ser con un string
//#define FIFOCC "servidor-hijo-cliente" // Nombre del archivo para enlace CC (Child-Client) (es un nombre de referencia únicamente)
//#define FIFOCC2 "servidor-hijo-cliente-2" // **
#define NUM_CLIENTES 2

int main(void){
	int n, fd, i;
	int fdcc1, fdcc2; // fdcc: file descriptor child-client
	char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.
	unlink(FIFONAME); // Elimina "my-fifo" si existe.
	//unlink(FIFOCC); // Elimina "servidor-hijo-cliente" si existe.

	/**
	* Quiza sean 3 tuberias, dos para comunicar a cada hijo con padre, y otra para la comunicacion de hijos con servidor.
	* Hay que revisarlo.
	*/

	if(mkfifo(FIFONAME,0666)<0){ // Crea archivo "servidor-padre-hijo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if((fd = open(FIFONAME,O_RDWR))<0){ // Se abre el archivo "servidor-padre-hijo" con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	for(i=0; i<NUM_CLIENTES; i++) {

    	// Acciones hijos
		if(fork()==0) {

			switch(i) {

				case 0: //Cliente 1 (jugador 1)
				   
			        if((fdcc1 = open(FIFOCC,O_RDWR))<0){ // Se abre el archivo "servidor-hijo-cliente" con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
				
				        perror("open"); // Si falla, error
				        exit(1);
			        }
				    break;

				case 1: //Cliente 2 (jugador 2)

			        if((fdcc2 = open(FIFOCC,O_RDWR))<0){ // Se abre el archivo "servidor-hijo-cliente" con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
				
				        perror("open"); // Si falla, error
				        exit(1);
			        }
				    break;

				default: exit (1);        
			}

			

	        while((n = read(fd,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos
		        
		        write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
		        // Acciones
		    }
	    	// Más cosas -------------
		    close(fd); // Va?? verificar
		    exit(1);
		}
	}
	
	//acciones padre (no deberia ningun hijo llegar aquí)
	else {
		if((fd=open(FIFONAME,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		    
		    perror("open"); // Si falla, error
		    exit(1);
	    }
	    //más cosas -----------
	}
	
	exit(0);
}
