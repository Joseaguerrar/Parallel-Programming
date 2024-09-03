//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef READ_H
#define READ_H

/**
 * @file read.h
 * @brief Funciones para la entrada/salida de datos en la simulación de transferencia de calor.
 */

/**
 * @brief Lee los datos desde un archivo y almacena la matriz de temperaturas y parámetros de simulación.
 * 
 * @param file Nombre del archivo de entrada.
 * @param filas Puntero a un entero donde se almacenará el número de filas de la matriz.
 * @param columnas Puntero a un entero donde se almacenará el número de columnas de la matriz.
 * @param epsilon Puntero a un `double` donde se almacenará el valor de epsilon (criterio de convergencia).
 * @param dtime Puntero a un `double` donde se almacenará el diferencial de tiempo (Δt).
 * @param alpha Puntero a un `double` donde se almacenará la difusividad térmica del material.
 * @param height Puntero a un `double` donde se almacenará el tamaño de las celdas (h).
 * @return double** Un puntero doble que apunta a la matriz de temperaturas.
 */
double ** read_file(const char *file, int *filas, int *columnas,
double *epsilon, double *dtime, double *alpha, double *height);

/**
 * @brief Imprime el resultado de la simulación en un archivo.
 * 
 * @param file Nombre del archivo de salida.
 * @param matriz Puntero doble que apunta a la matriz de temperaturas.
 * @param filas Número de filas de la matriz.
 * @param columnas Número de columnas de la matriz.
 */
void print_result(const char *file, double **matriz, int filas, int columnas);

#endif  //  READ_H
