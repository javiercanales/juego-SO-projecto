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

void imprimirMatriz(char* M);

int main (void){
	int n, fd, contador=0;
	char buf[1024]; // Cadena de char usado para guardar lo que se escribe en el cliente.
	
	printf("Conectando en jugador 1...\n");
	if((fd=open(FIFOC1,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		printf("Espacio jugador 1 ocupado --------------------\n");
		printf("Conectando en jugador 2...\n");
		if((fd=open(FIFOC2,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
			perror("open"); // Si falla, error
			exit(1);
		} else {
			printf("Conexion lograda - Jugador 2 - Preparando...\n");
			write(fd, "aqui", 4);
		}
	} else {
		printf("Conexion lograda - Jugador 1 - Preparando...\n");
		contador++;
		write(fd, "aqui", 4);
	}
	printf("Condiciones del juego:\n");
	printf("- Usted debera elegir una coordenada de la matriz entregada\n");
	printf("- Para las filas (vertical) se representa con letras\n");
	printf("- Las letras van desde la A hasta la E\n");
	printf("- Para las columnas (horizonal) se representa con numeros\n");
	printf("- Los numeros van desde el 0 al 4\n");
	printf("- Asi, si desea elegir el primer cuadrado, debera ingresar A1\n");
	printf("Mucha suerte! Espere unos segundos...\n");

    sleep(3);
    while((n = read(fd,buf,sizeof(buf)))>0) { // Lee mensajes desde el servidor, lo almacena en buf y retorna en número de bytes escritos
        printf("Cargando ------------------------\n");
		if(strncmp ("turno", buf, 5) == 0) { // Si es su turno (definimos el $ para indicar turno)

			printf("Es su turno!, ingrese su eleccion --------\n");
			// Acciones para mostrar matriz juego (substring? o identificar quizá)
			n = read(0,buf,sizeof(buf)); // Lee desde teclado el mensaje que se quiere enviar al servidor.
            write(fd,buf,n); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
            printf("\n-------- Procesando --------\n");
		} else if (strncmp("HIT!", buf, 4) == 0) {
			printf("HIT!\n");
			write(fd,"listo",n); //Confirma

		} else if (strncmp("MISS!", buf, 5) == 0){
			printf("MISS!\n");
			write(fd,"listo",n); //Confirma

		} else if (strncmp("ganador", buf, 7) == 0){
			printf("HIT ganador! Has ganado!\nFelicitaciones!!! ---------- Hasta la próxima\n");
			write(fd,"listo",n); //Antes de irse, avisa
			exit(0);
		} else if (strncmp("perdedor", buf, 8) == 0){
			printf("Has perdido!!! Mas suerte la proxima vez.\nFinalizando ----------\n");
			exit(0);
		}
		else { // Se envia la matriz de juego o un mensaje
			imprimirMatriz(buf);
			/*
			if (contador == 1) {
			    write(fd, "recibido", 8); // Avisa latido de la primera vez
			    contador--;
			    sleep(5); // Solo una vez (inicial) este sleep
		    }
		    */
		    printf("Espere su turno --------\n");
		}
		sleep(3);
	}
	printf("Finalizando partida...\n");
	close(fd);
	exit(0);
}

void imprimirMatriz(char* M) { 
	int i, contador = 0;
	printf("Imprimiendo matriz de juego --------\n\n");
	for(i=0; i<25; i++){
		contador++;
		printf(" %c ", M[i]);
		if(contador == 5 || contador == 10 || contador == 15 || contador == 20) {
            printf("\n");
		}
	}
	printf("\n\n--------------------------------\n");
}

