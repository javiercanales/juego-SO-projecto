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
#include <sys/wait.h> 
#include <signal.h>
#include <sys/sem.h> // Header de diferentes funciones para crear, operar y destruir semáforos
/**
* En este sistema se crean 3 tuberias, la primera, creada por el padre (con mkfifo(FIFONAME,0666))
* La segunda y tercera, creadas por
*/

#define FIFONAME "myfifo" // Nombre que se le va a dar al archivo al ser creado. No necesariamente definirlo de esta forma, también puede ser con un string
#define FIFOC1 "myfifo-cliente-1" // Nombre del archivo para enlace cliente 1 (es un nombre de referencia únicamente)
#define FIFOC2 "myfifo-cliente-2" // ** cliente 2
#define NUM_CLIENTES 2
#define TRUE 1

struct sembuf p = {0, -1, SEM_UNDO}; // Estructura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
                                     // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
                                     // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
                                     // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
                                    // que 1 significa que se va a aumentar el contador del semáforo.
void generar_mapa_inicial(char* M);
void analizar(char* buffer, char* M, char* respuesta);
void terminar1();
void terminar2();
static int fd, fd1, fd2;

int main(void){
    int n, i, semid;
    char M1[25]; // Se simula matriz como arreglo de tamaño 5*5 = 25
    char M2[25];

    char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.
    char *respuesta = malloc(10); // Avisa HIT! o MISS!

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
    printf("check 1\n");
    if(semid < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
        
        perror("semget"); exit(1);
    }
    if(semctl(semid, 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
                                          // el valor del contador (SETVAL) el valor 1.
        perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
    }
    printf("check 2\n");
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
    printf("check 3\n");

    generar_mapa_inicial(M1); // Matriz jugador 1
    generar_mapa_inicial(M2); // Matriz jugador 2

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
                    printf("check 4\n");
                    signal(SIGINT, terminar1);
                    printf("check 5\n");
                    n = read(fd1,buf,sizeof(buf)); // Se lee para detectar una conexion entrante (cliente) (un latido)
                    printf("check 6\n");
                    if(strncmp ("aqui", buf, 4) == 0) { // Se evalua que el latido sea correcto 
                                                           // (podria requerir cambios este metodo, o mejoras, ir evaluando)
                        printf("Jugador 1 conectado!\n");
                    } else {
                        printf("check 7\n");
                        printf("error, el buffer es: %s", buf);
                        exit(1);
                    }
                    printf("check 8\n");
                    chmod(FIFOC1, 0000); // Una vez detectada conexion, se cambia permiso asignado a la tuberia, para que no entren mas clientes.
                    // Quiza sirva hacer el chmod 0000 en el cliente, habra que probar (seria mas eficiente para evitar errores)
                    /**
                    * POR AQUI PODEMOS IR METIENDO SEÑALES PA AVISAR TURNOS
                    * ONDA se definesignal(SIGALRM, cambioturno())
                    * luego se llama alarm(1) pa detonar cambioturno().
                    * MEJOR AUN, USAR raise pa lanzar SIGUSR. Ver codigo señal.c
                    */

                    

                    printf("Esperando conexion cliente 2...\n");
                    n = read(fd,buf,sizeof(buf)); // Esperando respuesta proceso conexion cliente 2 (no debe ser critica porque debe estar en paralelo al otro proceso, esperando)
                    
                    /** Sección crítica
                    * Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                    * Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                    * se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                    * inmediatamente, esencialmente para el read(), que su uso sea sincrono (en este programa).
                    */
                    if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
                        perror("semop p-wait"); exit(1);
                    }

                    printf("Iniciando control de partida 1v1 --------\n");
                    printf("------------------------------------------------\n");

                    printf("Enviando matriz inicial a jugador 1 --------\n");
                    write(fd1, M1, sizeof(M1)); // Envia matriz asignada
                    sleep(3);

                    n = read(fd1, buf, sizeof(buf)); // Espera respuesta
                    printf("Matriz enviada a jugador 1 --------\n");
                    printf("Avisando nuevo turno jugador 1...\n");
                    printf("Esperando coordenada jugador 1...\n");

                    write(fd1, "$", 1); // Avisa turno inicial
                    sleep(3);

                    n = read(fd1, buf, sizeof(buf)); // Vuelve a esperar respuesta (1er turno)

                    printf("Coordenada (1 vez) recibida de jugador 1, analizando  --------\n");
                    analizar(buf, M1, respuesta);

                    printf("Enviando respuesta a jugador 1 --------\n");
                    write(fd1, respuesta, sizeof(respuesta));
                    sleep(3);

                    n = read(fd1, buf, sizeof(buf)); // Espera latido
                    printf("Respuesta recibida en jugador 1 --------\n");

                    printf("Enviando matriz a jugador 1 --------\n");
                    write(fd1, M1, sizeof(M1)); // Envia matriz asignada

                    printf("Aviso inicial a desde 1 a conexion de jugador 2 cambio turno --------\n");
                    write(fd, "$", 1); // Avisa cambio turno

                    if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
                        perror("semop v-post"); exit(1);
                    }
                    printf("------------------------------------------------\n");
                    sleep(3); // Se duerme 1s para sincronia tuberia fd
                    /* Fin sección */

                    while(TRUE) { 
                        /** Sección crítica
                        * Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                        * Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                        * se logra que cada proceso tenga acceso individual a la lectura (apoyados por sleep(3)). Uno libera el recurso, y el otro lo toma
                        * inmediatamente, esencialmente para el read() (en este programa).
                        */

                        if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
                            perror("semop p-wait"); exit(1);
                        }

                        n = read(fd,buf,sizeof(buf)); // Si hubiese un mensaje de ganador en contrincante
                        if(strncmp("derrota", buf, 7) == 0) {
                            write(fd1, "Has perdido!", 12);
                            printf("Juego terminado. Finalizando...\n");
                            exit(0);
                        }

                        printf("Nuevo turno jugador 1 --------\n");
                        write(fd1, "$", 1); // Avisa turno
                        sleep(3);

                        printf("Esperando coordenada jugador 1 --------\n");
                        n = read(fd1,buf,sizeof(buf)); // Espera coordenada

                        printf("Coordenada recibida de jugador 1, analizando  --------\n");
                        analizar(buf, M1, respuesta);

                        printf("Enviando respuesta a jugador 1 --------\n");
                        write(fd1, respuesta, sizeof(respuesta));
                        sleep(3);

                        printf("Esperando confirmacion jugador 1 --------\n");
                        n = read(fd1,buf,sizeof(buf)); // Espera latido

                        printf("Enviando matriz jugador 1 --------\n");
                        write(fd1, M1, sizeof(M1)); // Envia matriz asignada

                        if(strncmp("ganador", respuesta, 7) == 0){
                            printf("Informando triunfo de jugador 1 a jugador 2 --------\n");
                            write(fd, "derrota", 7); // Avisa a jugador 2 su derrota
                        } else {
                            write(fd, "nada", 7); // Mensaje para que el read() fd inicial no espere 
                        }

                        if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
                            perror("semop v-post"); exit(1);
                        }

                        printf("------------------------------------------------\n");
                        sleep(3); // Se duerme 3s para sincronia tuberia fd
                        /* Fin sección */
                    }
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
                    signal(SIGINT, terminar2);

                    n = read(fd2,buf,sizeof(buf)); // Se lee para detectar conexion entrante de cliente 2 (ocurrirá sólo cuando exista conexión en tuberia 1)
                    if(strncmp ("aqui", buf, 4) == 0) {
                        printf("Jugador 2 conectado!\n");
                    } else {
                        printf("error...\n");
                        exit(1);
                    }

                    printf("Enviando matriz a jugador 2\n");
                    write(fd2, M2, sizeof(M2)); // Envia matriz asignada
                    sleep(1);
                    printf("Matriz enviada a jugador 2\n");
                    /** Sección crítica
                    * Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                    * Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                    * se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                    * inmediatamente, esencialmente para el read() (en este programa).
                    */
                    if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
                        perror("semop p-wait"); exit(1);
                    }
                    printf("Avisando a conexion 1 de inicio juego (desde conexion 2)...\n");
                    write(fd, "init-game", 9); // Se avisa al proceso jugador 1 que los 2 clientes están conectados
                    
                    if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
                        perror("semop v-post"); exit(1);
                    }
                    /* Fin sección */
                    sleep(3); // Se duerme 1s para sincronia tuberia fd
                   
                    while(TRUE) { 
                        /** Sección crítica
                        * Se requiere sincronización por semáforo para el control de la lectura en la tuberia compartida/interna (fd)
                        * Sin sincronización, podría leerse datos generados por sí mismo (procesos). Con la sincronización por semáforo,
                        * se logra que cada proceso tenga acceso individual a la lectura. Uno libera el recurso, y el otro lo toma
                        * inmediatamente, esencialmente para el read() (en este programa).
                        */
                        if(semop(semid, &p, 1) < 0) { // sem_wait()), ocupa el recurso y restringe operaciones en otros procesos (proceso conexion jugador 2)
                            perror("semop p-wait"); exit(1);
                        }
                        
                        n = read(fd,buf,sizeof(buf)); // Si hubiese un mensaje de ganador en contrincante
                        if(strncmp("derrota", buf, 7) == 0) { // Aviso de ganador en contrincante
                            write(fd2, "Has perdido!", 12);
                            printf("Juego terminado. Finalizando...\n");
                            exit(0);
                        }

                        printf("Nuevo turno jugador 2 --------\n");
                        write(fd2, "$", 1); // Avisa turno inicial
                        sleep(3);

                        printf("Esperando coordenada jugador 2 --------\n");
                        n = read(fd2,buf,sizeof(buf)); // Espera respuesta

                        printf("Coordenada recibida de jugador 2, analizando  --------\n");
                        analizar(buf, M2, respuesta);
                        printf("Enviando respuesta a jugador 2 --------\n");
                        write(fd2, respuesta, sizeof(respuesta));
                        sleep(3);

                        printf("Esperando confirmacion jugador 2 --------\n");
                        n = read(fd2,buf,sizeof(buf)); // Espera latido

                        printf("Enviando matriz jugador 2 --------\n");
                        write(fd2, M2, sizeof(M2)); // Envia matriz asignada
                        sleep(3);

                        if (strncmp("ganador", respuesta, 7) == 0){
                            printf("Informando triunfo de jugador 2 a jugador 1 --------\n");
                            write(fd, "derrota", 7); // Avisa derrota
                            exit(0);
                        } else {
                            write(fd, "nada", 7); // Mensaje para que el read() fd inicial no espere 
                        }

                        if(semop(semid, &v, 1) < 0) { // Se libera el recurso para que otro proceso lo pueda ocupar (proceso conexion jugador 2)
                            perror("semop v-post"); exit(1);
                        }
                        printf("------------------------------------------------\n");
                        sleep(3); // Se duerme
                        /* Fin sección */
                    }

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
    return 0;
}

/* genera espacios vacios, luego, establece los barcos dentro de la matriz */
void generar_mapa_inicial(char *M){
    int i,k,r;
    for(i=0;i<25;i++){
        M[i]='-';
    }
    for(k=0;k<5;k++){
        do{
            r=rand() % 25;
        } while(M[r]=='B');
        M[r]='B';
    }
    return;
}
/* analiza coordenadas en buffer enviadas por jugador*/
//Ajuste al formato A1 (A inicial, 1 inicial, E final, 5 final)
void analizar(char* buffer, char* M, char* respuesta) {
    int y, posicionM, i, contador=0;
    char a, b;
    a = buffer[0];
    b = buffer[1];

    y = b - '0'; //char a int 

    if(y>4 || y<0){
        strcpy(respuesta, "MISS!");
        return;
    }

    switch(a) {
        case 'a':
        case 'A': posicionM = y;
            break;
        case 'b':
        case 'B': posicionM = y + 5;
            break;
        case 'c':
        case 'C': posicionM = y + 10;
            break;
        case 'd':
        case 'D': posicionM = y + 15;
            break;
        case 'e':
        case 'E': posicionM = y + 20;
            break;    
        default: strcpy(respuesta, "MISS!");
            return;         
    }

    if (M[posicionM] == 'B') {
        M[posicionM] = '-';
        strcpy(respuesta, "HIT!");
    } else {
        strcpy(respuesta, "MISS!");
    }

    if(strncmp("HIT!", respuesta, 4) == 0) {
        for(i=0; i<25; i++){
            if(M[i]=='B') {
                contador++;
            }
        }
        if(contador==0){
            printf("TENEMOS GANADOR!\n");
            strcpy(respuesta, "ganador");
        }
    }
}
void terminar1(){
    printf("Signal invocada, cerrando...\n");
    close(fd1);
    close(fd);
}
void terminar2(){
    printf("Signal invocada, cerrando...\n");
    close(fd2);
    close(fd);
}