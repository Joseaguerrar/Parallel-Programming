//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>

#include "matrix_operations.h"
#include <stdio.h>
#include <stdlib.h>

// Actualiza las temperaturas en la matriz de acuerdo con la fórmula de transferencia de calor
double update_temp(double **matriz, int filas, int columnas, double dtime, double alpha, double height) {
    double max_change = 0.0;
    double **new_matriz = asign_matrix(filas, columnas);

    for (int i = 1; i < filas - 1; ++i) {
        for (int j = 1; j < columnas - 1; ++j) {
            double delta = dtime * alpha / (height * height) *
                (matriz[i-1][j] + matriz[i][j+1] + matriz[i+1][j] + matriz[i][j-1] - 4 * matriz[i][j]);
            new_matriz[i][j] = matriz[i][j] + delta;

            if (abs(delta) > max_change) {
                max_change = abs(delta);
            }
        }
    }

    // Copiar las nuevas temperaturas a la matriz original
    for (int i = 1; i < filas - 1; ++i) {
        for (int j = 1; j < columnas - 1; ++j) {
            matriz[i][j] = new_matriz[i][j];
        }
    }

    free_matrix(new_matriz, filas);
    return max_change;
}

/**
 * @brief Asigna memoria para una matriz de tamaño filas x columnas.
 *
 * Esta función reserva espacio en memoria para una matriz de tamaño especificado,
 * donde cada fila es un array de punteros a double. La memoria para las columnas 
 * también es asignada dinámicamente.
 *
 * @param filas Número de filas de la matriz.
 * @param columnas Número de columnas de la matriz.
 * 
 * @return Un puntero doble a la matriz de double que ha sido asignada en memoria.
 * Si la asignación de memoria falla, el comportamiento es indefinido.
 */
double** asign_matrix(int filas, int columnas) {
    // Reservamos espacio para las filas (apuntan a arrays de doubles)
    double **matriz = (double **)malloc(filas * sizeof(double *));
    for (int i = 0; i < filas; i++) {
        // Reservamos espacio para las columnas (apuntan a doubles)
        matriz[i] = (double *)malloc(columnas * sizeof(double));
    }
    return matriz;
}

/**
 * @brief Libera la memoria asignada para una matriz de double.
 *
 * Esta función libera la memoria asignada para cada fila de la matriz 
 * y luego libera la memoria asignada para la matriz misma.
 *
 * @param matriz Puntero doble a la matriz de double que se va a liberar.
 * @param filas Número de filas de la matriz.
 */
void free_matrix(double **matriz, int filas) {
    for (int i = 0; i < filas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}
