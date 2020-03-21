
// Cliente
#include<time.h>


int main (void){

srand(time(NULL));
int M[5][5];

/*
Para la matriz, consideramos los valores de M como:
0: para espacio vacio
1: para barco
2: para barco hundido
3: para un disparo fallido
*/


}


int imprimir_mapa(int M[5][5]){
	for(i=0;i<5;i++){
		for(j=0;j<5;j++){
			g=M[i][j];
			/* g toma los valores de la matriz*/
			switch(g){
				case 0:
					printf("-");
					break;
				case 1:
					printf("B");
					break;
				case 2:
					printf("X");
					break;	
				case 3:
					printf("O")
			}
			printf("\t");
		}
		printf("\n");
	}
	
}

/*la funcion disparo pide parametros por teclado para luego devolverlo como un vector de dimmension 2 */
int disparo(void){
	int d[2];
	int x, y;
	printf("Apunte los cañones...\n")
	printf("Introduza coordenada del eje X:\n");
	scanf("%d", &x);
	printf("Introduzca coordenada del eje Y:\n");
	scanf("%d", &y);
	d[1]=x;
	d[2]=y;
	return(d)
}

/* genera espacios vacios, luego, establece los barcos dentro de la matriz*/
void generar_mapa_inicial(int M[][5], int filas){
	int r;
	int k;

	for(i=0;i<filas;i++){
		for(j=0;j<5;j++){
			M[i][j]=0;
		}
	}
	for(k=0;k<filas;k++){
		do{
			r=rand() % 5;
			k=rand() % 5;
		} while(M[r][k]==1);
		M[r][k]=1;
	}
	return;
}

int recibir_disparo(int d[2]){
	int s;
	if(M[d[1]][d[2]==1){
		s=1; /* HIT! */
	}
	
	else{
		s=2; /* MISS! */
	}

	switch (s){
		case 1:
			M[d[1]][d[2]]=2;
			printf("Te han hundido un barco! :(");
			break;

		case 2:
			M[d[1]][d[2]]=3;
			printf("Uff! Eso estuvo cerca...");
			break;
	}

