//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <matrix_operations.h>
#include <stdlib.h>
#include <math.h>

//  implementación para la actualización de temperatura de la matriz
double update_temp(double **matriz, int filas, int columnas, double dtime,
double alpha, double height) {
    //  variable para guardar el cambio máximo de temperatura de la matriz
    double max_change = 0.0;

    //  Crear una matriz temporal para almacenar los nuevos valores
    double **temp_matriz = asign_matrix(filas, columnas);

    //  Actualizar las temperaturas en la matriz, ignorando los bordes
    for (int i = 1; i < filas - 1; i++) {
        for (int j = 1; j < columnas - 1; j++) {
            //  Fórmula dada
            double new_temp = matriz[i][j] + (dtime * alpha / (height * height))
            * (matriz[i-1][j] + matriz[i+1][j] + matriz[i][j-1] +
            matriz[i][j+1] - (4 * matriz[i][j]));

            temp_matriz[i][j] = new_temp;
            //  Calcular el cambio máximo
            double change = fabs(new_temp - matriz[i][j]);
            //  fabs es para el valor absoluto de un flotante
            //  si hay un mayor cambio hacemos el swap
            if (change > max_change) {
                max_change = change;
            }
        }
    }

    //  Copiar la matriz temporal a la original
    for (int i = 1; i < filas - 1; i++) {
        for (int j = 1; j < columnas - 1; j++) {
            matriz[i][j] = temp_matriz[i][j];
        }
    }
    //  Liberar matriz
    free_matrix(temp_matriz, filas);
    return max_change;
}

//  implementación para la asignación de la matriz
double** asign_matrix(int filas, int columnas) {
    //  Reservamos espacio para las filas(apuntan a arrays de doubles)
    double **matriz = (double **)malloc(filas * sizeof(double *));
    for (int i = 0; i < filas; i++) {
        //  Reservamos espacio para las columnas(apuntan a doubles)
        matriz[i] = (double *)malloc(columnas * sizeof(double));
    }
    return matriz;
}

//  implementación para liberar la matriz de memoria
void free_matrix(double **matriz, int filas) {
    for (int i = 0; i < filas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}


