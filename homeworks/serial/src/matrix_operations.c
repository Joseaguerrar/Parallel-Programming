#include <matrix_operations.h>
#include <stdlib.h>
#include <math.h>

//implementación para la actualización de temperatura de la matriz
double update_temp(double **matriz, int filas, int columnas, double dtime,
double alpha, double height, double epsilon) {
    //variable para guardar el cambio máximo de temperatura de la matriz
    double max_change = 0.0;

    // Crear una matriz temporal para almacenar los nuevos valores
    double **temp_matriz = asign_matrix(filas, columnas);
}

//implementación para la asignación de la matriz
double** asign_matrix(int filas, int columnas) {
    //Reservamos espacio para las filas(apuntan a arrays de doubles)
    double **matriz = (double **)malloc(filas * sizeof(double *));
    for (int i = 0; i < filas; i++) {
        //Reservamos espacio para las columnas(apuntan a doubles)
        matriz[i] = (double *)malloc(columnas * sizeof(double));
    }
    return matriz;
}

//implementación para liberar la matriz de memoria
void free_matrix(double **matriz, int filas) {
    for (int i = 0; i < filas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}


