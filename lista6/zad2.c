#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
//gcc -pthread zad2.c -o zad2


//2 macierze + macierz wynikowa
typedef struct {
	int size;
	int** matrix1;
	int** matrix2;
	int** result; 
} matrices;

//curr_low - który wiersz aktualnie się wykonuje
int curr_row = 0;
//za pomocą mutexa będziemy blokować (synchronizowac) nasze watki tak, zeby dwa nie robily tego samego wiersza
pthread_mutex_t mutex;

//alokowanie pamięci i wypełnianie macierzy zerami i jedynkami
int **allocate_mat(int size)
{
	srand(time(NULL));
	int **m = calloc(size, sizeof(int*));
	for(int i = 0; i<size; i++)
	{
		m[i] = calloc(size, sizeof(int));
		for(int j = 0; j<size; j++)
		{
			m[i][j] = rand()%2; 
		}
	}
	return m;
}
void* mult(void* vargp) {
	matrices* m = (matrices*) vargp;
	
	while (curr_row < m->size) {

		pthread_mutex_lock(&mutex);
		int row = curr_row;
		curr_row++;
		pthread_mutex_unlock(&mutex);
		
		for (int i = 0; i < m->size; i++) {
			for (int j = 0; j < m->size; j++) {
				m->result[row][i] |= (m->matrix1[row][j] & m->matrix2[j][i]);
				if (m->result[row][i] == 1) break;
			}
		}
	}
	
	pthread_exit(0);
}
//mnożenie macierzy
/*
void *mult(void *args)
{
	matrices *m = (matrices*) args;	
	
	while(curr_row < m->size)
	{
		pthread_mutex_lock(&mutex);		
		int r = curr_row; 
		curr_row++;
		pthread_mutex_unlock(&mutex);
		for(int i = 0; i < m->size; i++)
		{
	    		for(int j = 0; j < m->size; j++)
			{	
				m->result[i][j] |= m->matrix1[i][r] && m->matrix2[r][j];		
				if(m->result[i][j]==1)break;
			}
		
		}
	}
	pthread_exit(0);

}*/

void write_matrix(int **matrix, int size)
{
	for(int i = 0; i<size; i++)
	{
	
		for(int j = 0; j<size; j++)
		{
			printf("%d| ",matrix[i][j]);
		}
		printf("\n");
	
	}
	printf("\n");
}




int main(int argc, char *argv[])
{

	if(argc < 3)
	{
		printf("1. size of single matrix\n2.number of threads\n");
		return 1;
	}

	int size = atoi(argv[1]);
	int thr = atoi(argv[2]);
	//printf("%d\n	",size);
	pthread_t threads[thr];
	matrices mats; 
	mats.size = size;
	mats.matrix1 = allocate_mat(size);
	mats.matrix2 = allocate_mat(size);
	write_matrix(mats.matrix1,size);
	write_matrix(mats.matrix2,size);
	mats.result = calloc(size, sizeof(int*));
	for(int i = 0; i<size; i++)mats.result[i] = calloc(size,sizeof(int));
	pthread_mutex_init(&mutex, NULL);
	for(int i = 0; i<thr; i++)
	{
		printf("Włączam wątek numer %d\n",i);
		//pthread_create - odpal nowy wątek
		pthread_create(&threads[i], NULL, &mult, &mats);
	}
	for(int i = 0;i<thr; i++)
	{
		//pthread_join - czekaj na koniec procesu 
		pthread_join(threads[i],NULL);
		printf("Wątek numer %d został zakończony\n",i);
	}
	write_matrix(mats.result,size);
	pthread_mutex_destroy(&mutex);
	printf("zakończono program\n");
	



	return 0;
}
