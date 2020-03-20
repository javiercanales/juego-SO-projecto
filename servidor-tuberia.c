/**
* Tuberia.c
* Tuberias con nombre: programa para el servidor.
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
#define FIFOC1 "myfifo-cliente-1" // Nombre del archivo para enlace cliente 1 (es un nombre de referencia únicamente)
#define FIFOC2 "myfifo-cliente-2" // ** cliente 2
#define NUM_CLIENTES 2

int main(void){
	int n, fd, fd1, fd2, i;
	char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.
	unlink(FIFONAME); // Elimina "my-fifo" si existe.
	unlink(FIFOC1); // Elimina "myfifo-cliente-1" si existe.
	unlink(FIFOC2); // Elimina "myfifo-cliente-2" si existe.


	if(mkfifo(FIFONAME,0666)<0){ // Crea archivo "servidor-padre-hijo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}
	if((fd = open(FIFONAME,O_RDWR))<0){ // Se abre el archivo "servidor-padre-hijo" con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	
	printf("Jugador conectado! Resta uno mas...\n");
	//Bloquear a cliente conectado mientras se espera
	n = read(fd,buf,sizeof(buf));

	for(i=1; i <= NUM_CLIENTES; i++) {

    	// Acciones hijos
		if(fork()==0) {

			switch(i) {

				case 1: //Cliente 1 (jugador 1)

				    printf("Iniciando conexion con jugadores\n");
				    if(mkfifo(FIFOC1,0666)<0){ // Crea archivo "myfifo-cliente-1" con permisos 666, si no existe lo crea.
		                perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		                exit(1);
	                }
	                if((fd1 = open(FIFOC1,O_RDWR))<0){ // Se abre el archivo con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		                perror("open"); // Si falla, error
		                exit(1);
	                }

				    n = read(fd1,buf,sizeof(buf)); // Se lee para detectar una conexion entrante (cliente)
				    chmod(FIFOC1, 0000); // Vez detectada conexion, se cambia permiso asignado a la tuberia, para que no entren mas clientes.
				    printf("Jugador 1 conectado!\n");
				    // Quiza sirva hacer el chmod 0000 en el cliente, habra que probar (seria mas eficiente para evitar errores)
				    
		            while((n = read(fd1,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos
		        
		                write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
		                // Acciones
		            }
	    	        // Más cosas -------------
		            close(fd1); // Va?? verificar

				    break;

				case 2: //Cliente 2 (jugador 2)

                    if(mkfifo(FIFOC2,0666)<0){ // Crea archivo "myfifo-cliente-2" con permisos 666, si no existe lo crea.
		                perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		                exit(1);
	                }
	                if((fd2 = open(FIFOC2,O_RDWR))<0){ // Se abre el archivo con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		                perror("open"); // Si falla, error
		                exit(1);
	                }

				    n = read(fd2,buf,sizeof(buf)); // Se lee para detectar conexion entrante de cliente 2 (ocurrirá sólo cuando exista conexión en tuberia 1)
				    printf("Jugador 2 conectado!\n");

                    while((n = read(fd2,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos
		        
		                write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
		                // Acciones
		            }
	    	        // Más cosas -------------
		            close(fd2); // Va?? verificar

				    break;

				default: exit(1);        
			}
		    exit(1);
		}

	}
	
	//acciones padre (no deberia ningun hijo llegar aquí)
	if((fd=open(FIFONAME,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		    
		perror("open"); // Si falla, error
		exit(1);
	}

	//más cosas -----------
	while((n=read(fd,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos
		write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
	}
	
	close(fd);
	exit(0);
}
