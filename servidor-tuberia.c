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
#include <string.h>
/**
* En este sistema se crean 3 tuberias, la primera, creada por el padre (con mkfifo(FIFONAME,0666))
* La segunda y tercera, creadas por
*/

#define FIFONAME "myfifo" // Nombre que se le va a dar al archivo al ser creado. No necesariamente definirlo de esta forma, también puede ser con un string
#define FIFOC1 "myfifo-cliente-1" // Nombre del archivo para enlace cliente 1 (es un nombre de referencia únicamente)
#define FIFOC2 "myfifo-cliente-2" // ** cliente 2
#define NUM_CLIENTES 2
struct sembuf p = {0, -1, SEM_UNDO}; // Estructura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
									 // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
									 // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
									 // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
									// que 1 significa que se va a aumentar el contador del semáforo.
void generar_mapa_inicial(int M[][5], int filas);

int main(void){
	int n, fd, fd1, fd2, i, semid;
	int M1[5][5];
	int M2[5][5];
	char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.

	unlink(FIFONAME); // Elimina "my-fifo" si existe.
	unlink(FIFOC1); // Elimina "myfifo-cliente-1" si existe.
	unlink(FIFOC2); // Elimina "myfifo-cliente-2" si existe.

    /**
    * Definicion de semáforos, para sincronización de procesos, con fin de controlar accion
    * de procesos padre (servidor general) e hijos (servicio a conexiones/clientes)
    */
    semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT); // Obtiene la id de un arreglo de semáforos, el primero argumento de semget()
													  // define la visibilidad del arreglo con respecto a otro procesos que distinta jerarquia, en
													  // este caso, es privado, solo procesos de la misma jerarquia podrá visualizar el semáforo.
													  // 1 significa la dimensión del arreglo que semáforos
													  // El ultimo parametro da los permisos.

    if(semid < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
    	
        perror("semget"); exit(1);
    }
    if(semctl(semid, 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
										  // el valor del contador (SETVAL) el valor 1.
        perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
    }

    /**
    * Definicion de archivos que dan funcionamiento a la tuberia con nombre.
    * Esta tuberia comunica al padre con sus hijos gracias a la sincronización que ofrece
    * el uso de semáforos, lo que produce un acceso secuenciado por turnos.
    */
	if(mkfifo(FIFONAME,0666)<0){ // Crea archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}
	if((fd = open(FIFONAME,O_RDWR))<0){ // Se abre el archivo "myfifo" con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}
	generar_mapa_inicial(M1, 25); // Matriz jugador 1
	generar_mapa_inicial(M2, 25); // Matriz jugador 2

	printf("Iniciando servidor...\n");
	for(i=1; i <= NUM_CLIENTES; i++) {

    	// Acciones hijos
		if(fork()==0) {

			switch(i) {

				case 1: //Cliente 1 (jugador 1)

				    printf("Iniciando conexion con jugadores --------\n");
				    if(mkfifo(FIFOC1,0666)<0){ // Crea archivo "myfifo-cliente-1" con permisos 666, si no existe lo crea.
		                perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		                exit(1);
	                }
	                if((fd1 = open(FIFOC1,O_RDWR))<0){ // Se abre el archivo con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		                perror("open"); // Si falla, error
		                exit(1);
	                }

				    n = read(fd1,buf,sizeof(buf)); // Se lee para detectar una conexion entrante (cliente) (un latido)
                    if(strcmp ("aqui-cliente", buf) == 0) { // Se evalua que el latido sea correcto 
                    	                                   // (podria requerir cambios este metodo, o mejoras, ir evaluando)
				    	printf("Jugador 1 conectado!\n");
				    } else {
				    	exit(1);
				    }

				    chmod(FIFOC1, 0000); // Una vez detectada conexion, se cambia permiso asignado a la tuberia, para que no entren mas clientes.
				    // Quiza sirva hacer el chmod 0000 en el cliente, habra que probar (seria mas eficiente para evitar errores)
				    
				    n = read(fd,buf,sizeof(buf)); // Esperando respuesta proceso conexion cliente 2 (no debe ser critica porque debe estar en paralelo al otro proceso)
				    
	                printf("Iniciando control de partida 1v1 --------\n");
                    printf("Sincronizando sistema --------");

                    write(fd1, M1, sizeof(M1)); // Envia matriz asignada
                    n = read(fd1, buf, sizeof(buf)); // Espera respuesta
                    write(fd1, "$", 1); // Avisa turno inicial
                    n = read(fd1,buf,sizeof(buf)); // Vuelve a esperar respuesta (1er turno)
                    // Se evalua respuesta, y se gestiona
                    write(fd1, M1, sizeof(M1)); // Envia matriz asignada

                    /** Sección crítica
                    * Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                    * Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                    * se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                    * inmediatamente, esencialmente para el read(), que su uso sea sincrono (en este programa).
                    */
				    if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
		                perror("semop p-wait"); exit(1);
                    }

                    write(fd, "$", 1); // Avisa cambio turno

                    if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
		                perror("semop v-post"); exit(1);
	                }
	                /* Fin sección */

                    // QUIZÁ SEA NECESARIO USAR PEQUEÑOS WAIT BREVES (PAUSAS, STOP, ETC).
                    // EVALUAR BIEN EL USO DE LOS SEMAFOROS
                    // LA ESTRUCTURA PARA LA SINCRONIA ESTA BIEN, PERO QUIZA LOS METODOS NO, PUEDE QUE HAYA
                    // QUE UTILIZAR LOS SEMAFOROS DE OTRA FORMA, O COMBINAR CON ALGO (CON waits ES BASTANTE FEO, PERO
                    // SI NO QUEDA OTRA, SERA NOMAS CON wait) 
                    // (Sleep) (Quiza mezclar con SEÑALES)
                    // OJO, ES IMPORTANTE VERIFICAR QUE SE DE LA SINCRONIA DE ACCESO AL READ (FD) CORRECTAMENTE.
                    
                    n = read(fd,buf,sizeof(buf)); // Lee desde proceso conexion 2
                    do { 
                    	/** Sección crítica
                    	* Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                    	* Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                    	* se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                    	* inmediatamente, esencialmente para el read() (en este programa).
                    	*/

                        if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
		                    perror("semop p-wait"); exit(1);
                        }

                        if (strcmp ("$"), buf) { // Si es aviso de nuevo turno
                    		write(fd1, "$", 1); // Avisa turno inicial
                    		n = read(fd1,buf,sizeof(buf)); // Espera respuesta
                    		// Se evalua respuesta, y se gestiona
                    		write(fd1, M1, sizeof(M1)); // Envia matriz asignada
                            write(fd, "$", 1); // Avisa cambio turno
                    	}
                    	else { // Sino, es aviso de ganador en contrincante
                    		// Aviso de ganador en jugador 2, informa derrota a jugador 1
                    	}

		                if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
		                    perror("semop v-post"); exit(1);
	                    }
	                    /* Fin sección */
		            } while ((n = read(fd,buf,sizeof(buf)))>0); // Espera nuevo turno (aviso desde proceso conexion 2)
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
				    if(strcmp ("aqui-cliente", buf) == 0) {
				    	printf("Jugador 2 conectado!\n");
				    } else {
				    	exit(1);
				    }

                    write(fd2, M2, sizeof(M2)); // Envia matriz asignada
				    /** Sección crítica
                    * Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                    * Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                    * se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                    * inmediatamente, esencialmente para el read() (en este programa).
                    */
				    if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
		                perror("semop p-wait"); exit(1);
                    }

                    write(fd, "init-game", 9); // Se avisa al proceso jugador 1 que los 2 clientes están conectados (ocurre sólo en tal caso para este sistema)
                    
                    if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
		                perror("semop v-post"); exit(1);
	                }
	                /* Fin sección */

	                n = read(fd, buf, sizeof(buf)); // Lee desde proceso conexion 1
                    do { 
                    	/** Sección crítica
                    	* Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                    	* Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                    	* se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                    	* inmediatamente, esencialmente para el read() (en este programa).
                    	*/
                    	if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
		                    perror("semop p-wait"); exit(1);
                        }

                        if (strcmp ("$"), buf) { // Si es aviso de nuevo turno
                    		write(fd2, "$", 1); // Avisa turno inicial
                    		n = read(fd2,buf,sizeof(buf)); // Espera respuesta
                    		// Se evalua respuesta, y se gestiona
                    		write(fd2, M2, sizeof(M2)); // Envia matriz asignada
                            write(fd, "$", 1); // Avisa cambio turno
                    	}
                    	else { // Sino, es aviso de ganador en contrincante
                    		// Aviso de ganador en jugador 2, informa derrota a jugador 1
                    	}

		                if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
		                    perror("semop v-post"); exit(1);
	                    }
	                    /* Fin sección */
		            } while ((n = read(fd,buf,sizeof(buf)))>0); // Espera nuevo turno (aviso desde proceso conexion 1)

				    break;

				default: exit(1);        
			}
		    exit(1);
		}

	}

	// Acciones padre (Control general partida del servidor) --- no se requiere derivar de un "else" del "if (fork() == 0)"
	// (como hemos hecho en ejemplos) pues los hijos nunca llegarán aquí (por diseño general del código)
    // Por ahora no se define nada, se revisará si es necesario (aplicar nueva tuberia quizá, o esperar a hijos con wait NULL)
    for(i=0; i<2; i++) {
        wait(NULL);
    }
	printf("Juego finalizado!\n");

	close(fd);
	exit(0);
}

/* genera espacios vacios, luego, establece los barcos dentro de la matriz */
void generar_mapa_inicial(int *M, int dimension){

	for(i=0;i<dimension;i++){
		M[i]=0;
	}
	for(k=0;k<5;k++){
		do{
			r=rand() % dimension;
		} while(M[r]==1);
		M[r]=1;
	}
	return;
}
