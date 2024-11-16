//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <omp.h>

#include "heat_simulation.h"

/**
 * @brief Lee el archivo binario correspondiente a cada lámina y ejecuta la simulación de transferencia de calor.
 * 
 * Esta función abre un archivo binario que contiene los datos iniciales de una lámina, ejecuta la simulación 
 * de transferencia de calor y genera el resultado final en un nuevo archivo binario. Al finalizar todas las 
 * simulaciones, se genera un reporte con los resultados de todas las láminas.
 * 
 * @param folder Carpeta donde se encuentran los archivos binarios.
 * @param variables Arreglo de estructuras params_matrix que contiene los parámetros de cada simulación.
 * @param lines Número de simulaciones a realizar.
 * @param jobName Nombre del archivo de trabajo.
 * @param num_threads Cantidad de hilos para la simulación.
 */
void read_bin_plate(const char* folder,
                    params_matrix* variables,
                    uint64_t lines,
                    const char* jobName,
                    int num_threads) {
    FILE *bin_file;
    uint64_t rows, columns;
    char direction[512];

    // Crear un arreglo para almacenar los estados por cada simulación
    uint64_t* array_state_k = malloc(lines * sizeof(uint64_t));
    if (array_state_k == NULL) {
        fprintf(stderr,
                      "Error al asignar memoria para el arreglo de estados.\n");
        return;
    }

    for (uint64_t i = 0; i < lines; i++) {
        // Construir la ruta del archivo binario
        snprintf(direction, sizeof(direction),
                                        "%s/%s", folder, variables[i].filename);

        // Abrir el archivo binario para leer
        bin_file = fopen(direction, "rb");
        if (bin_file == NULL) {
            fprintf(stderr,
             "No se pudo abrir el archivo binario %s\n", variables[i].filename);
            continue;  // Continuar con la siguiente simulación en caso de error
        }

        // Leer el número de filas y columnas de la matriz
        fread(&rows, sizeof(uint64_t), 1, bin_file);
        fread(&columns, sizeof(uint64_t), 1, bin_file);

        // Asignar memoria para la matriz
        double **matrix = malloc(rows * sizeof(double*));
        if (matrix == NULL) {
            fprintf(stderr,
                      "Error al asignar memoria para las filas de la matriz\n");
            fclose(bin_file);
            continue;
        }

        for (uint64_t j = 0; j < rows; j++) {
            matrix[j] = malloc(columns * sizeof(double));
            if (matrix[j] == NULL) {
                fprintf(stderr,
                   "Error al asignar memoria para las columnas de la matriz\n");
                for (uint64_t k = 0; k < j; k++) {
                    free(matrix[k]);
                }
                free(matrix);
                fclose(bin_file);
                continue;
            }

            // Leer cada fila del archivo binario y almacenarla en la matriz
            size_t elements_read = fread(matrix[j], sizeof(double),
                                                             columns, bin_file);
            if (elements_read != columns) {
                fprintf(stderr, "Error al leer los datos de la fila %lu\n", j);
                for (uint64_t k = 0; k <= j; k++) {
                    free(matrix[k]);
                }
                free(matrix);
                fclose(bin_file);
                continue;
            }
        }
        // Cerrar el archivo binario después de leer los datos
        fclose(bin_file);
        // Ejecutar la simulación y capturar el número de estados
        uint64_t states_k = heat_transfer_simulation(matrix, rows, columns,
                                                     variables[i].delta_t,
                                                     variables[i].alpha,
                                                     variables[i].h,
                                                     variables[i].epsilon,
                                                     num_threads);

        // Guardar el número de estados en el arreglo
        array_state_k[i] = states_k;

        // Generar archivo binario con el estado final
        generate_bin_file(matrix, rows, columns, folder,
                                       variables[i].filename, array_state_k[i]);

        // Liberar la memoria de la matriz
        for (uint64_t j = 0; j < rows; j++) {
            free(matrix[j]);
        }
        free(matrix);
    }

    // Generar el archivo de reporte con todos los resultados
    generate_report_file(folder, jobName, variables, array_state_k, lines);

    // Liberar el arreglo de estados
    free(array_state_k);
}

/**
 * @brief Realiza la simulación de transferencia de calor en una matriz.
 * 
 * Esta función toma como entrada una matriz que representa una lámina, así como otros parámetros físicos y 
 * de simulación, y ejecuta la transferencia de calor hasta que se alcanza el equilibrio térmico. 
 * Utiliza múltiples hilos para dividir el trabajo en filas de la matriz.
 * 
 * @param matrix Matriz de la lámina con los datos iniciales.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @param delta_t Diferencial de tiempo.
 * @param alpha Difusividad térmica.
 * @param h Tamaño de las celdas.
 * @param epsilon Sensitividad del punto de equilibrio.
 * @param num_threads Cantidad de hilos de ejecución.
 * 
 * @return Número de estados hasta alcanzar el punto de equilibrio.
 */
uint64_t heat_transfer_simulation(double** matrix, uint64_t rows, uint64_t columns,
                                  double delta_t, double alpha, double h, double epsilon, int num_threads) {
    omp_set_num_threads(num_threads); // Configura el número de hilos

    double** new_matrix = create_empty_matrix(rows, columns);
    if (new_matrix == NULL) return 0;

    uint64_t total_states = 0;
    bool balance_point = false;

   while (!balance_point) {
    balance_point = true;
    double coef = alpha * delta_t / (h * h);  // Constante calculada antes del bucle

    // Definir tamaño de bloque
    int block_size = 5;  // Cambia según las necesidades de balance de carga

    #pragma omp parallel shared(matrix, new_matrix, balance_point, coef)
    {
        // Actualización de las celdas con mapeo cíclico por bloque
        #pragma omp for schedule(static, block_size)
        for (uint64_t i = 1; i < rows - 1; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                double new_temp = matrix[i][j] + coef *
                    (matrix[i-1][j] + matrix[i+1][j] +
                     matrix[i][j-1] + matrix[i][j+1] - 4 * matrix[i][j]);
                new_matrix[i][j] = new_temp;

                if (fabs(new_temp - matrix[i][j]) >= epsilon) {
                    #pragma omp critical
                    balance_point = false;  // Actualización segura de balance_point
                }
            }
        }

        // Barrera explícita para sincronizar antes de copiar matrices
        #pragma omp barrier

        // Copiar los valores de `new_matrix` a `matrix` para la próxima iteración
        #pragma omp for schedule(static, block_size)
        for (uint64_t i = 0; i < rows; i++) {
            for (uint64_t j = 0; j < columns; j++) {
                matrix[i][j] = new_matrix[i][j];
            }
        }

        // Barrera implícita al final del bucle paralelo
    }

    total_states++;
}



    free_matrix(new_matrix, rows);
    return total_states;
}

/**
 * @brief Crea una matriz vacía de tamaño especificado.
 * 
 * Asigna memoria para una matriz bidimensional con el número de filas y columnas dado.
 * 
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * 
 * @return Puntero a la matriz creada o NULL si no se pudo asignar memoria.
 */
double** create_empty_matrix(uint64_t rows, uint64_t columns) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    if (matrix == NULL) {
        return NULL;  // Manejar error de asignación
    }
    for (uint64_t i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(columns * sizeof(double));
        if (matrix[i] == NULL) {
            // Si falla la asignación, liberar la memoria previamente asignada
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
 * @brief Copia una matriz en otra.
 * 
 * Esta función copia los datos de una matriz fuente a una matriz destino.
 * 
 * @param dest_matrix Matriz de destino.
 * @param src_matrix Matriz fuente.
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
 * @brief Libera la memoria utilizada por una matriz.
 * 
 * Esta función libera la memoria asignada dinámicamente para una matriz bidimensional.
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
