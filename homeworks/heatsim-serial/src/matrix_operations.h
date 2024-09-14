//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include <time.h>  // Incluir time.h para definir time_t
#include <stddef.h>  // Incluir stddef.h para definir size_t

/**
 * @file matrix_operations.h
 * @brief Funciones para la manipulación de matrices y el procesamiento de archivos de simulación.
 */

/**
 * @brief Actualiza las temperaturas en la matriz de acuerdo con la fórmula de transferencia de calor.
 * 
 * @return double El cambio máximo de temperatura en la matriz durante esta actualización.
 */
double update_temp(double **matriz, int filas, int columnas, double dtime,
double alpha, double height);

/**
 * @brief Crea una matriz bidimensional con el número especificado de filas y columnas.
 * 
 * @return double** Un puntero doble que apunta a la matriz creada.
 */
double** asign_matrix(int filas, int columnas);

/**
 * @brief Libera el espacio de memoria ocupado por una matriz bidimensional.
 */
void free_matrix(double **matriz, int filas);

/**
 * @brief Procesa el archivo de trabajo para realizar las simulaciones.
 */
void process_job_file(const char *job_file, const char *output_dir);

/**
 * @brief Formatea el tiempo transcurrido en un formato legible.
 */
char* format_time(const time_t seconds, char* text, const size_t capacity);

#endif  //  MATRIX_OPERATIONS_H
