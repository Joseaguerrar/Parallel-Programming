//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

/**
 * @file matrix_operations.h
 * @brief Funciones para la manipulación de matrices en la simulación de transferencia de calor.
 */

/**
 * @brief Actualiza las temperaturas en la matriz de acuerdo con la fórmula de transferencia de calor.
 * 
 * @param matriz Puntero doble que apunta a la matriz de temperaturas.
 * @param filas Número de filas en la matriz.
 * @param columnas Número de columnas en la matriz.
 * @param dtime Diferencial de tiempo (Δt).
 * @param alpha Difusividad térmica del material.
 * @param height Tamaño de las celdas (h).
 * @param epsilon Valor de epsilon que representa el criterio de convergencia.
 * @return double El cambio máximo de temperatura en la matriz durante esta actualización.
 */
double update_temp(double **matriz, int filas, int columnas, double dtime,
double alpha, double height);

/**
 * @brief Crea una matriz bidimensional con el número especificado de filas y columnas.
 * 
 * @param filas Número de filas en la matriz.
 * @param columnas Número de columnas en la matriz.
 * @return double** Un puntero doble que apunta a la matriz creada.
 */
double** asign_matrix(int filas, int columnas);

/**
 * @brief Libera el espacio de memoria ocupado por una matriz bidimensional.
 * 
 * @param matriz Puntero doble que apunta a la matriz de temperaturas.
 * @param filas Número de filas en la matriz.
 */
void free_matrix(double **matriz, int filas);

#endif  //  MATRIX_OPERATIONS_H
