#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO 500
#define SECAO 4

// Função que povoa uma matriz qualquer, de uma ordem qualquer, com um valor qualquer
void criar_matriz(int matrix[TAMANHO][TAMANHO], int valor) {
	for (int i = 0; i < TAMANHO;  i++) {
		for (int j = 0; j < TAMANHO; j++) {
			matrix[i][j] = valor;
			matrix[i][j] = valor;
		}	
	}
}

// Função que imprime uma dada matriz
void print_matriz(int matrix[TAMANHO][TAMANHO]) {
	for (int i = 1; i < TAMANHO;  i++) {
		for (int j = 1; j < TAMANHO; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}

// Função de soma das matrizes
void soma_matriz(int matrix_A[TAMANHO][TAMANHO], int matrix_B[TAMANHO][TAMANHO], int matrix_C[TAMANHO][TAMANHO], int token) {
	for(int i = token; i < TAMANHO; i+=SECAO) {
		for (int j = 0; j < TAMANHO; j++) {
			matrix_C[j][i] = matrix_A[j][i] + matrix_B[j][i];
		}
	}
}


// Função que junta matrizes
void juntar_matriz(int matrix_A[TAMANHO][TAMANHO], int matrix_B[TAMANHO][TAMANHO], int token) {
	for(int i = token; i < TAMANHO; i+=SECAO) {
		for (int j = 0; j < TAMANHO; j++) {
			matrix_A[j][i] = matrix_B[j][i];
		}
	}
}

// Função que inicia uma matriz com valores aleatórios
void criar_matriz_random(int matrix[TAMANHO][TAMANHO]) {
	srand(time(NULL));
	for (int i = 0; i < TAMANHO;  i++) {
		for (int j = 0; j < TAMANHO; j++) {
			matrix[i][j] = rand() % 100;
		}	
	}
}

int main(int argc, char** argv) {
	// Inicia o ambiente do MPI
	MPI_Init(NULL, NULL);

	// Captura o número de processos que serão utilizados na execução
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Captura o número total de processos
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int matrixA[TAMANHO][TAMANHO];
	int matrixB[TAMANHO][TAMANHO];
	int matrixC[TAMANHO][TAMANHO];

    criar_matriz_random(matrixA)
    criar_matriz_random(matrixB)
    criar_matriz_random(matrixC)


	if(world_rank == 0) {
		// Mostra as matrizes A, B e C
		printf("Printando matriz  A: \n");
		print_matriz(matrixA);
		printf("Printando matriz B: \n");
		print_matriz(matrixB);
		printf("Printando matriz C: \n");
		print_matriz(matrixC);

		// Envia coluna de onde deve iniciar cada subprocesso
		int token = 0;
		MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		token++;
		MPI_Send(&token, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		token++;
		MPI_Send(&token, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);

		// Soma o que sobrou da matriz
		token++;
		soma_matriz(matrixA, matrixB, matrixC, token);

		// Aguarda mensagem dos processos que estão somando a matriz
		int matrix_temp[TAMANHO][TAMANHO];
		MPI_Recv(&matrix_temp, TAMANHO*TAMANHO, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("0: Recebendo dados do processo 1.\n");
		token = 0;
		juntar_matriz(matrixC, matrix_temp, token);

		MPI_Recv(&matrix_temp, TAMANHO*TAMANHO, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("0: Recebendo dados do processo 2.\n");
		token++;
		juntar_matriz(matrixC, matrix_temp, token);

		MPI_Recv(&matrix_temp, TAMANHO*TAMANHO, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("0: Recebendo dados do processo 3.\n");
		token++;
		juntar_matriz(matrixC, matrix_temp, token);

		// Mostra os resultados na matriz C
		printf("MATRIZ C: \n");
		print_matriz(matrixC);
	}
	else {
		int token;
		MPI_Recv(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		soma_matriz(matrixA, matrixB, matrixC, token);
		MPI_Send(matrixC, TAMANHO*TAMANHO, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	// Finaliza e limpa o ambiente MPI
	MPI_Finalize();
}