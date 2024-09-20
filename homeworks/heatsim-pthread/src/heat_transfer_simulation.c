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

    // Crear un arreglo para almacenar los estados por cada simulación
    uint64_t* array_state_k = malloc(lines * sizeof(uint64_t));
    if (array_state_k == NULL) {
        fprintf(stderr, "Error al asignar memoria para el arreglo de estados.\n");
        return;
    }

    for (uint64_t i = 0; i < lines; i++) {
        snprintf(direction, sizeof(direction), "%s/%s", folder, variables[i].filename);
        bin_file = fopen(direction, "rb");
        if (bin_file == NULL) {
            fprintf(stderr, "No se pudo abrir el archivo binario %s\n", variables[i].filename);
            continue;  // Continuar con la siguiente simulación en caso de error
        }

        fread(&rows, sizeof(uint64_t), 1, bin_file);
        fread(&columns, sizeof(uint64_t), 1, bin_file);

        double **matrix = malloc(rows * sizeof(double*));
        if (matrix == NULL) {
            fprintf(stderr, "Error al asignar memoria para las filas de la matriz\n");
            fclose(bin_file);
            continue;  // Liberar recursos y continuar con la siguiente simulación
        }

        for (uint64_t j = 0; j < rows; j++) {
            matrix[j] = malloc(columns * sizeof(double));
            if (matrix[j] == NULL) {
                fprintf(stderr, "Error al asignar memoria para las columnas de la matriz\n");
                for (uint64_t k = 0; k < j; k++) {
                    free(matrix[k]);
                }
                free(matrix);
                fclose(bin_file);
                continue;  // Liberar recursos y continuar con la siguiente simulación
            }
        }

        // Obtener los parámetros para la simulación
        double delta_t = variables[i].delta_t;
        double alpha = variables[i].alpha;
        double h = variables[i].h;
        double epsilon = variables[i].epsilon;

        // Ejecutar la simulación y capturar el número de estados
        uint64_t states_k = heat_transfer_simulation(matrix, rows, columns, delta_t, alpha, h, epsilon, num_threads);

        // Guardar el número de estados en el arreglo
        array_state_k[i] = states_k;

        // Generar archivo binario con el estado final
        generate_bin_file(matrix, rows, columns, folder, variables[i].filename, states_k);

        // Liberar la memoria de la matriz
        for (uint64_t j = 0; j < rows; j++) {
            free(matrix[j]);
        }
        free(matrix);
        fclose(bin_file);
    }

    // Generar el archivo de reporte con todos los resultados
    generate_report_file(folder, jobName, variables, array_state_k, lines);

    // Liberar el arreglo de estados
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
    pthread_t threads[num_threads];  // Array para los hilos
    private_data thread_args[num_threads];  // Array para los datos privados de cada hilo
    shared_data shared;  // Datos compartidos entre los hilos

    // Inicializar los datos compartidos
    shared.balance_point = false;  // Inicializar el punto de equilibrio como falso
    shared.matrix = matrix;  // La matriz original es compartida entre los hilos

    // Inicializar los datos privados de cada hilo
    for (int t = 0; t < num_threads; t++) {
        thread_args[t].states_k = 0;
    }

    // Simulación de transferencia de calor
    while (!shared.balance_point) {
        // Reiniciar el indicador de equilibrio global al principio de cada iteración

        uint64_t rows_per_thread = (rows - 2) / num_threads; // Filas que le toca a cada hilo

        for (int t = 0; t < num_threads; t++) {
            uint64_t start_row = 1 + t * rows_per_thread;
            uint64_t end_row = (t == num_threads - 1) ? rows - 1 : start_row + rows_per_thread;
            thread_args[t].start_row = start_row;
            thread_args[t].end_row = end_row;
            thread_args[t].columns = columns;
            thread_args[t].delta_t = delta_t;
            thread_args[t].alpha = alpha;
            thread_args[t].h = h;
            thread_args[t].epsilon = epsilon;
            thread_args[t].shared = &shared;  // Pasar el puntero a los datos compartidos

            // Crear el hilo
            pthread_create(&threads[t], NULL, heat_transfer_simulation_thread, &thread_args[t]);
        }

        // Esperar a que todos los hilos terminen
        for (int t = 0; t < num_threads; t++) {
            pthread_join(threads[t], NULL);
        }

    }
    // Sumar los estados de todos los hilos
    uint64_t total_states_k = 0;
    for (int t = 0; t < num_threads; t++) {
        total_states_k += thread_args[t].states_k;
    }

    printf("Cantidad total de estados: %lu\n", total_states_k);  // Imprimir la cantidad total de estados
    return total_states_k;  // Devolver el número total de estados
}
void* heat_transfer_simulation_thread(void* arg) {
    private_data* data = (private_data*)arg;
    //data->shared->balance_point = true;
    // Cada hilo realiza su tarea y actualiza directamente la matriz compartida (original)
    data->states_k++;  // Incrementar el contador local de estados
    printf("Estados = %lu\n", data->states_k);  // Imprimir los estados de cada hilo
    // Calcular las nuevas temperaturas para las celdas asignadas a este hilo
    for (uint64_t i = data->start_row; i < data->end_row; i++) {
        for (uint64_t j = 1; j < data->columns - 1; j++) {
            double new_temp = data->shared->matrix[i][j] + 
                              data->alpha * data->delta_t / (data->h * data->h) *
                              (data->shared->matrix[i-1][j] + data->shared->matrix[i+1][j] + 
                               data->shared->matrix[i][j-1] + data->shared->matrix[i][j+1] - 
                               4 * data->shared->matrix[i][j]);

            // Actualizar directamente la matriz original (shared)
            data->shared->matrix[i][j] = new_temp;

            // Verificar si aún no se ha alcanzado el equilibrio
            if (fabs(new_temp - data->shared->matrix[i][j]) > data->epsilon) {
                data->shared->balance_point = false;  // Aún no se alcanza el equilibrio
            }
            else if (fabs(new_temp - data->shared->matrix[i][j]) < data->epsilon)
            {
                data->shared->balance_point = true;
            }
        
            
        }
    }

    return NULL;
}
