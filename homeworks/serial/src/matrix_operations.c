//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <matrix_operations.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief Actualiza las temperaturas en una matriz utilizando un esquema de diferencias finitas.
 *
 * Esta función actualiza las temperaturas en una matriz basada en un esquema de diferencias finitas,
 * ignorando los bordes de la matriz. Calcula el nuevo valor de temperatura para cada celda interna 
 * utilizando los valores de sus vecinos y almacena el cambio máximo de temperatura durante la iteración.
 *
 * @param matriz Puntero doble a la matriz de temperaturas.
 * @param filas Número de filas en la matriz.
 * @param columnas Número de columnas en la matriz.
 * @param dtime Paso de tiempo utilizado en la simulación.
 * @param alpha Coeficiente de difusividad térmica.
 * @param height Altura o tamaño del paso espacial.
 *
 * @return El cambio máximo de temperatura registrado durante la iteración.
 */
double update_temp(double **matriz, int filas, int columnas, double dtime,
double alpha, double height) {
    // Variable para guardar el cambio máximo de temperatura de la matriz
    double max_change = 0.0;

    // Crear una matriz temporal para almacenar los nuevos valores
    double **temp_matriz = asign_matrix(filas, columnas);

    // Actualizar las temperaturas en la matriz, ignorando los bordes
    for (int i = 1; i < filas - 1; i++) {
        for (int j = 1; j < columnas - 1; j++) {
            // Fórmula dada
            double new_temp = matriz[i][j] + (dtime * alpha / (height * height))
            * (matriz[i-1][j] + matriz[i+1][j] + matriz[i][j-1] +
            matriz[i][j+1] - (4 * matriz[i][j]));

            temp_matriz[i][j] = new_temp;
            // Calcular el cambio máximo
            double change = fabs(new_temp - matriz[i][j]);
            // fabs es para el valor absoluto de un flotante
            // Si hay un mayor cambio, lo actualizamos
            if (change > max_change) {
                max_change = change;
            }
        }
    }

    // Copiar la matriz temporal a la original
    for (int i = 1; i < filas - 1; i++) {
        for (int j = 1; j < columnas - 1; j++) {
            matriz[i][j] = temp_matriz[i][j];
        }
    }

    // Liberar la matriz temporal
    free_matrix(temp_matriz, filas);

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
