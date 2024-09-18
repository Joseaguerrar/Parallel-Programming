//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

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
                    const char* jobName,
                    int num_threads) {
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

        // Obtener los parámetros para la simulación
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
                                                    epsilon, num_threads);
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
                                    double epsilon,
                                    int num_threads) {
    pthread_t threads[num_threads]; // Creamos array con hilos
    private_data thread_args[num_threads]; // Creamos array con datos privados para cada hilo
    shared_data shared; // Creamos la estructura para datos compartidos

    // Inicializar datos compartidos
    pthread_mutex_init(&shared.mutex, NULL); // Inicializar el mutex
    shared.balance_point = false;
    shared.states_k = malloc(sizeof(uint64_t)); // Asignar memoria para states_k
    *(shared.states_k) = 0; // Inicializar el contador de estados a 0

    // Crear una matriz temporal para guardar los cambios de las celdas internas
    double** temp_matrix = malloc(rows * sizeof(double*));
    for (uint64_t i = 0; i < rows; i++) {
        temp_matrix[i] = malloc(columns * sizeof(double));
    }

    // Simulación de transferencia de calor
    while (!shared.balance_point) {
        shared.balance_point = true; // Inicializar el balance_point como true

        uint64_t rows_per_thread = (rows - 2) / num_threads; // Filas que le toca a cada hilo

        for (int t = 0; t < num_threads; t++) {
            uint64_t start_row = 1 + t * rows_per_thread;
            uint64_t end_row = (t == num_threads - 1) ? rows - 1 : start_row + rows_per_thread;

            // Asignamos los datos privados
            thread_args[t].matrix = matrix;
            thread_args[t].temp_matrix = temp_matrix;
            thread_args[t].start_row = start_row;
            thread_args[t].end_row = end_row;
            thread_args[t].columns = columns;
            thread_args[t].delta_t = delta_t;
            thread_args[t].alpha = alpha;
            thread_args[t].h = h;
            thread_args[t].epsilon = epsilon;
            thread_args[t].shared = &shared; // Asignar el puntero a los datos compartidos

            // Crear el hilo
            pthread_create(&threads[t], NULL, heat_transfer_simulation_thread, &thread_args[t]);
        }

        // Esperar a que todos los hilos terminen
        for (int t = 0; t < num_threads; t++) {
            pthread_join(threads[t], NULL);
        }

        // Actualizar la matriz original con los valores de la matriz temporal
        for (uint64_t i = 1; i < rows - 1; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                matrix[i][j] = temp_matrix[i][j];
            }
        }
    }

    // Liberar la memoria de la matriz temporal
    for (uint64_t i = 0; i < rows; i++) {
        free(temp_matrix[i]);
    }
    free(temp_matrix);

    // Guardar el valor final de states_k
    uint64_t final_states_k = *(shared.states_k);

    // Liberar memoria y destruir el mutex
    free(shared.states_k);
    pthread_mutex_destroy(&shared.mutex);

    return final_states_k;
}
