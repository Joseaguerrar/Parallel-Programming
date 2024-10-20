//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "heat_simulation.h"

/**
 * @brief Lee el archivo binario correspondiente a cada lámina y ejecuta la simulación de transferencia de calor.
 * 
 * @param folder Carpeta donde se encuentran los archivos binarios.
 * @param variables Arreglo de estructuras `params_matrix` que contiene los parámetros de cada simulación.
 * @param lines Número de simulaciones a realizar.
 * @param jobName Nombre del archivo de trabajo.
 */
void read_bin_plate(const char* folder,
                    params_matrix* variables,
                    uint64_t lines,
                    const char* jobName) {
    FILE *bin_file;
    uint64_t rows, columns;
    char direction[512];
    uint64_t* array_state_k = malloc(lines * sizeof(uint64_t));

    for (uint64_t i = 0; i < lines; i++) {
        snprintf(direction, sizeof(direction), "%s/%s",
                    folder, variables[i].filename);
        bin_file = fopen(direction, "rb");
        if (bin_file == NULL) {
            fprintf(stderr, "No se pudo abrir el archivo binario %s\n",
                                                    variables[i].filename);
            return;
        }

        // Leer el número de filas y columnas
        fread(&rows, sizeof(uint64_t), 1, bin_file);
        fread(&columns, sizeof(uint64_t), 1, bin_file);

        double **matrix = malloc(rows * sizeof(double*));
        if (matrix == NULL) {
            fprintf(stderr,
                    "Error al asignar memoria para las filas de la matriz\n");
            fclose(bin_file);
            return;
        }

        for (uint64_t i = 0; i < rows; i++) {
            matrix[i] = malloc(columns * sizeof(double));
            if (matrix[i] == NULL) {
                fprintf(stderr,
                   "Error al asignar memoria para las columnas de la matriz\n");
                for (uint64_t j = 0; j < i; j++) {
                    free(matrix[j]);
                }
                free(matrix);
                fclose(bin_file);
                return;
            }
        }

        // Leer los valores de la matriz desde el archivo binario
        for (uint64_t i = 0; i < rows; i++) {
            fread(matrix[i], sizeof(double), columns, bin_file);
        }

        // Simulación de transferencia de calor
        double delta_t = variables[i].delta_t;
        double alpha = variables[i].alpha;
        double h = variables[i].h;
        double epsilon = variables[i].epsilon;
        uint64_t states_k = heat_transfer_simulation(matrix,
                                                    rows,
                                                    columns,
                                                    delta_t,
                                                    alpha,
                                                    h,
                                                    epsilon);
        array_state_k[i] = states_k;

        // Generar archivo binario con el estado final
        generate_bin_file(matrix,
                            rows,
                            columns,
                            folder,
                            variables[i].filename,
                            states_k);

        // Liberar la memoria de la matriz
        for (uint64_t i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
        fclose(bin_file);
    }

    // Generar el archivo de reporte con los resultados
    generate_report_file(folder, jobName, variables, array_state_k, lines);
    free(array_state_k);
}

/**
 * @brief Realiza la simulación de transferencia de calor en una matriz.
 * 
 * @param matrix Matriz de la lámina con los datos iniciales.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @param delta_t Diferencial de tiempo.
 * @param alpha Difusividad térmica.
 * @param h Tamaño de las celdas.
 * @param epsilon Sensitividad del punto de equilibrio.
 * 
 * @return Número de estados hasta alcanzar el punto de equilibrio.
 */
uint64_t heat_transfer_simulation(double** matrix,
                                  uint64_t rows,
                                  uint64_t columns,
                                  double delta_t,
                                  double alpha,
                                  double h,
                                  double epsilon) {
     // Crear dos matrices: matrix_a y matrix_b
    double** matrix_a = create_empty_matrix(rows, columns);
    double** matrix_b = create_empty_matrix(rows, columns);

    // Copiar el estado inicial de matrix a matrix_a y matrix_b
    copy_matrix(matrix_a, matrix, rows, columns);
    copy_matrix(matrix_b, matrix, rows, columns);

    bool balance_point = false;
    uint64_t states_k = 0;
    // Simulación de transferencia de calor
    while (!balance_point) {
        balance_point = true;
        // Alternar entre matrix_a y matrix_b
        double** current_matrix = (states_k % 2 == 1) ? matrix_a : matrix_b;
        double** next_matrix = (states_k % 2 == 1) ? matrix_b : matrix_a;

        for (uint64_t i = 1; i < rows - 1; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                double new_temperature = current_matrix[i][j] +
                    ((delta_t * alpha) / (h * h)) * (current_matrix[i-1][j] +
                                                     current_matrix[i+1][j] +
                                                     current_matrix[i][j-1] +
                                                     current_matrix[i][j+1] -
                                                     4 * current_matrix[i][j]);

                next_matrix[i][j] = new_temperature;

                // Verificar si el cambio es mayor que epsilon
                if (fabs(new_temperature - current_matrix[i][j]) > epsilon) {
                    balance_point = false;
                }
            }
        }
        states_k++;
    }

    // Copiar el estado final a la matriz original
    copy_matrix(matrix, (states_k % 2 == 1) ? matrix_b :
                                                       matrix_a, rows, columns);

    // Liberar las matrices temporales
    free_matrix(matrix_a, rows);
    free_matrix(matrix_b, rows);

    return states_k;
    // Retornar el número de iteraciones hasta alcanzar el equilibrio
}

/**
 * @brief Crea una matriz vacía de tamaño rows x columns.
 * 
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @return Un puntero a la matriz vacía, o NULL si no se pudo asignar memoria.
 */
double** create_empty_matrix(uint64_t rows, uint64_t columns) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    if (matrix == NULL) {
        return NULL;  // Manejar error de asignación
    }
    for (uint64_t i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(columns * sizeof(double));
        if (matrix[i] == NULL) {
            for (uint64_t j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;  // Manejar error de asignación
        }
    }
    return matrix;
}

/**
 * @brief Copia el contenido de una matriz a otra.
 * 
 * @param dest_matrix Matriz de destino.
 * @param src_matrix Matriz de origen.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 */
void copy_matrix(double** dest_matrix, double** src_matrix, uint64_t rows,
                                                             uint64_t columns) {
    for (uint64_t i = 0; i < rows; i++) {
        memcpy(dest_matrix[i], src_matrix[i], columns * sizeof(double));
    }
}

/**
 * @brief Libera la memoria de una matriz.
 * 
 * @param matrix Matriz a liberar.
 * @param rows Número de filas de la matriz.
 */
void free_matrix(double** matrix, uint64_t rows) {
    for (uint64_t i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

/**
 * @brief Imprime el contenido de una matriz en la consola. Útil para depuración.
 * 
 * @param matrix Matriz a imprimir.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 */
void print_matrix(double** matrix, uint64_t rows, uint64_t columns) {
    for (uint64_t i = 0; i < rows; i++) {
        for (uint64_t j = 0; j < columns; j++) {
            printf("%8.4f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
