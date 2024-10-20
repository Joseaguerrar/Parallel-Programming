//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

#include "heat_simulation.h"

/**
 * @brief Lee el archivo binario correspondiente a cada lámina y ejecuta la simulación de transferencia de calor.
 * 
 * @param folder Carpeta donde se encuentran los archivos binarios.
 * @param variables Arreglo de estructuras params_matrix que contiene los parámetros de cada simulación.
 * @param lines Número de simulaciones a realizar.
 * @param jobName Nombre del archivo de trabajo.
 * @param num_threads Cantidad de hilos para el plate
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

uint64_t heat_transfer_simulation(double** matrix,
                                  uint64_t rows,
                                  uint64_t columns,
                                  double delta_t,
                                  double alpha,
                                  double h,
                                  double epsilon,
                                  int num_threads) {
    (void)epsilon;  // Ignorar parámetro no utilizado
    pthread_t threads[num_threads];
    private_data thread_args[num_threads];
    shared_data shared;

    // Inicializar los datos compartidos
    shared.balance_point = false;
    shared.global_matrix = matrix;

    // Calcular el coeficiente constante
    double coef_local = alpha * delta_t / (h * h);
    shared.coef = &coef_local;

    // Inicializar el contador de filas y el mutex
    shared.fila_actual = 1;
    shared.total_states = 0;
    pthread_mutex_init(&shared.mutex, NULL);

    // Inicializar los hilos
    for (int t = 0; t < num_threads; t++) {
        thread_args[t].columns = columns;
        thread_args[t].rows = rows;
        thread_args[t].local_coef = &coef_local;
        thread_args[t].shared = &shared;
        thread_args[t].local_matrix = create_empty_matrix(rows, columns);
        copy_matrix(thread_args[t].local_matrix, shared.global_matrix, rows, columns);
        pthread_create(&threads[t], NULL, heat_transfer_simulation_thread, &thread_args[t]);
    }

    // Esperar a que todos los hilos terminen
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    // Destruir el mutex
    pthread_mutex_destroy(&shared.mutex);

    // Liberar memoria
    for (int t = 0; t < num_threads; t++) {
        free_matrix(thread_args[t].local_matrix, rows);
    }

    // Devolver el número total de estados alcanzados
    return shared.total_states;
}

void* heat_transfer_simulation_thread(void* arg) {
    private_data* data = (private_data*)arg;
    
    // Copiar el coeficiente localmente para optimizar el acceso
    double coef_local = *(data->shared->coef);

    while (true) {
        uint64_t fila;

        // Proteger el acceso al contador de filas con un mutex
        pthread_mutex_lock(&(data->shared->mutex));
        fila = data->shared->fila_actual;
        data->shared->fila_actual++;

        // Verificar si ya no hay más filas para procesar
        if (fila >= data->rows - 1) {
            pthread_mutex_unlock(&(data->shared->mutex));
            break;
        }
        pthread_mutex_unlock(&(data->shared->mutex));

        // Calcular las nuevas temperaturas para las celdas en la fila obtenida
        for (uint64_t j = 1; j < data->columns - 1; j++) {
            double new_temp = data->local_matrix[fila][j] +
                              coef_local * (data->local_matrix[fila-1][j] + 
                                            data->local_matrix[fila+1][j] + 
                                            data->local_matrix[fila][j-1] + 
                                            data->local_matrix[fila][j+1] - 
                                            4 * data->local_matrix[fila][j]);

            data->local_matrix[fila][j] = new_temp;
        }

        // Incrementar el contador de estados de forma segura
        pthread_mutex_lock(&(data->shared->mutex));
        data->shared->total_states++;
        pthread_mutex_unlock(&(data->shared->mutex));
    }

    return NULL;
}


// Función para crear una matriz vacía de tamaño rows x columns
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

// Función para copiar una matriz a otra
void copy_matrix(double** dest_matrix, double** src_matrix, uint64_t rows, uint64_t columns) {
    for (uint64_t i = 0; i < rows; i++) {
        memcpy(dest_matrix[i], src_matrix[i], columns * sizeof(double));
    }
}

// Función para liberar la memoria de una matriz
void free_matrix(double** matrix, uint64_t rows) {
    for (uint64_t i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
