/* 
 * Semáforos: En programación son representados como un arreglo de semaforos.
 * Estos son referenciados por medio de una id, esta id tendrá n semáforos, este n se configura.
 * 
 * El caso sin semaforo el programa crea un proceso padre e hijo, estos se ejecutan en secuencia (en su mayoria) primero el padre luego el hijo.
 * El caso con semaforo el programa crea un proceso padre e hijo, estos se turnan para usar la pantalla, aqui se ve dos conceptos: recurso compartido y condición de carrera.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h> // Definición de macros	
#include <sys/sem.h> // Header de diferentes funciones para crear, operar y destruir semáforos

struct sembuf p = {0, -1, SEM_UNDO}; // Estrutura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
									 // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
									 // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
									 // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
									// que 1 significa que se va a aumentar el contador del semáforo.

int main() {
    pid_t pid = -1;
    int opcion, semid;
    
    printf("1 - Con semaforo\n2 - Sin semaforo\n\nIngrese su opción: ");
    scanf("%d",&opcion);
    
    if(opcion == 1) {

		goto semaforo;
	}
	else if(opcion == 2) {

		pid = fork();		
		for(int i = 0 ; i < 10 ; i++) {
			if(pid == 0) printf("Escribió el hijo\n");
			else printf("Escribió el padre\n");
		}

		return 0;
	}
	else {
		printf("\nIngrese una opción correcta\n");
		return 0;
	}
    
    semaforo:
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

    pid = fork();

	for(int i = 0 ; i < 10 ; i++) {
		if(semop(semid, &p, 1) < 0) { // semop realiza una operación sobre un arreglo de semaforos. Recibe la id del arreglo de semaforos,
									  // el puntero de la operación (en este caso, p, sem_wait()) y la dimensión del arreglo. Se ocupa el recurso.
			perror("semop p"); exit(1);
		}
		if(pid == 0) printf("Escribo el hijo\n"); // Linea 66 y 67, son las operaciones que, para este caso, ocupan el recurso o la sección critica. 
		else printf("Escribo el padre\n");		  // Suponiendo que el recurso es la pantalla.
		if(semop(semid, &v, 1) < 0) { // Luego se libera el recurso para que otro proceso lo pueda ocupar
			perror("semop v"); exit(1);
		}
	}

    return 0;

}
