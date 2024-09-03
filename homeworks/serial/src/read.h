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
 * @return double** Un puntero doble que apunta a la matriz de temperaturas.
 */
double ** read_file(const char *file, int *filas, int *columnas,
double *epsilon, double *dtime, double *alpha, double *height);

/**
 * @brief Imprime el resultado de la simulación en un archivo.
 * 
 */
void print_result(const char *file, double **matriz, int filas, int columnas);

#endif  //  READ_H
