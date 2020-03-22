/*
 * Cronometro con uso de señales: usar señales para mostrar el paso del tiempo por cada un segundo y también
 * la comunicación para que tanto el padre como el hijo tengan la misma cantidad de segundos pasados.
 * NOTA: La programación de señales son hederables, es decir, que si se programa una señal en el padre y luego se ejecuta fork, el hijo también
 * tendrá la señal programada.
 * NOTA 2: Las señales programas tambien se puede detonar desde un proceso externo (distinta jerarquia), si se conocer el pid del proceso, desde
 * la terminal de linux se puede ejecutar kill -<1-64> <pid>, donde <1-64> es el valor asociado a al señal (consultar "kill -l"). Por ejemplo, si se
 * quiere matar un proceso, sabiendo que 9 es el valor asociado a SIGKILL (señal que mata a un proceso) y el pid del proceso es 20.
 * Ejecutando "kill -9 20 matará al proceso 20"
 * */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int seg = 0;

void cronometro(){ // Función ejecutado desde el proceso hijo
	kill(getppid(),SIGUSR1); // La función kill no es una función que solamente mata a un proceso. Se encarga de mandar una señal a un proceso
							 // en particular por su pid. Existen 64 señales donde 2 son destinados a que el programar decida que hacer con ellos
							 // (SIGUSR1 y SIGUSR2). Puede averiguar todas escribiendo en el terminal de linux "kill -l".
							 // En este caso, desde el proceso hijo, se le manda una señal SIGUSR1 al proceso padre (getppid())
	printf("%d seg - Parar con Control-C\n",++seg);
	alarm(1); // Se programa la señal de alarma a un segundo (cada vez que se ejecuta esta función se volverá a programar)
}

void incrementar(){ // Función ejecutado desde el proceso padre
	seg++;
}



int main(){
	pid_t id_hijo;
	if((id_hijo = fork()) == 0) { // En el hijo...
		signal(SIGALRM,cronometro); // La función signal se encarga de asociar una señal a ejecutar una función. En este caso se programa la señal
									// SIGALRM a ejecutar la función "cronometro", es decir, cuando el proceso reciba la señal, se va a ejecutar "cronometro"
		alarm(1); // Función en que programa la detonación de la señal SIGALRM en 1 segundo.
		while(1);
	}else{ // En el padre...
		signal(SIGUSR1,incrementar); // Se programa la señal SIGUSR1 para que ejecute incrementar cuando reciba la señal.
		signal(SIGINT,SIG_IGN); // SIG_IGN es la función de ignorar, en esta caso se programa a que SIGINT (que se denota presionando Ctrl+C)
								// a ser ignorado haciendo que continue su ejecución.
		int estado;
		while(waitpid(id_hijo,&estado,0) != id_hijo); // Espera al hijo
		if(WIFSIGNALED(estado)){ // Verifica si al hijo fue señalizado de alguna forma.
			printf("\nTerminado en %d seg, Con estado %d\n",seg,estado);
		}
		exit(0);
	}
	return 0;
}
