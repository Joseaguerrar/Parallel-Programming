#include <stdio.h>
#include <stdlib.h>

int main() {
    int filas = 3;
    int columnas = 3;

    // Crear la matriz utilizando punteros
    int **matriz = (int **)malloc(filas * sizeof(int *));
    for(int i = 0; i < filas; i++) {
        matriz[i] = (int *)malloc(columnas * sizeof(int));
    }

    // Llenar la matriz
    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            matriz[i][j] = i + j;  // Puedes cambiar esta lógica para llenar la matriz como prefieras
        }
    }

    // Imprimir la matriz
    printf("La matriz es:\n");
    for(int i = 0; i < filas; i++) {
        for(int j = 0; j < columnas; j++) {
            printf("\n ");
            printf("\n %p",&matriz[i][j]);
        }
    }
}