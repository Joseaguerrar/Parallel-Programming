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
uint64_t heat_transfer_simulation(double** matrix,
                                  uint64_t rows,
                                  uint64_t columns,
                                  double delta_t,
                                  double alpha,
                                  double h,
                                  double epsilon,
                                  int num_threads) {
    // Array de hilos
    pthread_t threads[num_threads]; //NOLINT
    // Array de datos privados de cada hilo
    private_data thread_args[num_threads]; //NOLINT
    // Datos compartidos entre hilos
    shared_data shared;

    // Inicializar los datos compartidos
    shared.balance_point = false;
    shared.global_matrix = matrix;

    // **Optimización**: Calcular el coeficiente constante y almacenarlo en shared_data
    double coef_local = alpha * delta_t / (h * h);
    shared.coef = &coef_local;  // Almacenar la dirección del coeficiente en el campo `coef` de `shared_data`

    // Cantidad total de estados
    uint64_t total_states_k = 0;
    uint64_t rows_per_thread = (rows - 2) / num_threads;
    for (int t = 0; t < num_threads; t++) {
        uint64_t start_row = 1 + t * rows_per_thread;
        uint64_t end_row = (t == num_threads - 1) ? rows - 1 :
                                                    start_row + rows_per_thread;

        // Inicializar los datos del hilo
        thread_args[t].start_row = start_row;
        thread_args[t].end_row = end_row;
        thread_args[t].columns = columns;
        thread_args[t].rows = rows;
        thread_args[t].delta_t = delta_t;
        thread_args[t].alpha = alpha;
        thread_args[t].h = h;
        thread_args[t].epsilon = epsilon;
        thread_args[t].shared = &shared;
        thread_args[t].id = t;
        thread_args[t].local_coef = &coef_local;
        // Asignar una nueva matriz local para cada hilo
        thread_args[t].local_matrix = create_empty_matrix(rows, columns);
        copy_matrix(thread_args[t].local_matrix, shared.global_matrix,
                                                                 rows, columns);
    }

    /*Crear una matriz nueva donde se almacenarán
    los resultados al final de cada iteración*/
    double** new_matrix = create_empty_matrix(rows, columns);
    copy_matrix(new_matrix, shared.global_matrix, rows, columns);
    if (new_matrix == NULL) {
        // Retornar inmediatamente si no se puede crear la matriz
        return total_states_k;
    }

    // Simulación de transferencia de calor
    while (!shared.balance_point) {
        // Inicializar la variable como true al inicio de la iteración
        shared.balance_point = true;
        if (num_threads == 1) {
            // Solo hay un hilo, así que se hace de una vez
            // Actualizar la matriz local directamente
            copy_matrix(thread_args[0].local_matrix, shared.global_matrix, rows, columns);

            // Ejecutar la simulación de transferencia de calor directamente
            heat_transfer_simulation_thread(&thread_args[0]);

            // Copiar los cambios de la matriz local a la new_matrix
            copy_matrix(new_matrix, thread_args[0].local_matrix, rows, columns);
        } else {
            // Si hay más de un hilo, se sigue con el manejo normal de hilos

            // Actualizar la matriz local para cada hilo
            for (int t = 0; t < num_threads; t++) {
                copy_matrix(thread_args[t].local_matrix, shared.global_matrix, rows, columns);
            }

            // Crear los hilos para procesar las filas
            for (int t = 0; t < num_threads; t++) {
                pthread_create(&threads[t], NULL, heat_transfer_simulation_thread, &thread_args[t]);
            }

            // Esperar a que todos los hilos terminen
            for (int t = 0; t < num_threads; t++) {
                pthread_join(threads[t], NULL);
            }

            // Copiar los cambios de las matrices locales de los hilos a la new_matrix
            for (int t = 0; t < num_threads; t++) {
                for (uint64_t i = thread_args[t].start_row; i < thread_args[t].end_row; i++) {
                    // Copiar toda la fila usando memcpy
                    memcpy(new_matrix[i], thread_args[t].local_matrix[i], columns * sizeof(double));
                }
            }
        }
        // Verificar el balance point
        for (uint64_t i = 1; i < rows - 1; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                if (fabs(new_matrix[i][j] - shared.global_matrix[i][j]) >= epsilon) {
                    shared.balance_point = false;
                    break; // Salir del bucle de columnas
                }
            }
            if (!shared.balance_point) {
                break; // Salir del bucle de filas si ya se detectó una diferencia
            }
        }
        // Copiar la nueva matriz a la matriz global para la siguiente iteración
        copy_matrix(shared.global_matrix, new_matrix, rows, columns);
        total_states_k++;
    }
    // Liberar las matrices locales después de que los hilos hayan terminado
    for (int t = 0; t < num_threads; t++) {
        free_matrix(thread_args[t].local_matrix, rows);
    }
    // Liberar memoria de la matriz temporal
    free_matrix(new_matrix, rows);

    return total_states_k;  // Devolver el número total de estados
}

/**
 * @brief Función ejecutada por cada hilo para realizar la simulación de transferencia de calor en un rango de filas.
 * 
 * Esta función se encarga de calcular las nuevas temperaturas para las celdas asignadas a cada hilo
 * y actualiza directamente la matriz original compartida. También comprueba si se ha alcanzado el 
 * equilibrio térmico para las celdas procesadas por el hilo. Si se alcanza el equilibrio, se marca 
 * el balance_point global como verdadero.
 * 
 * @param arg Puntero a la estructura private_data que contiene la información necesaria para que el hilo procese su tarea.
 * 
 * @return NULL Siempre retorna NULL después de finalizar su trabajo.
 */
void* heat_transfer_simulation_thread(void* arg) {
    private_data* data = (private_data*)arg;
    
    // **Optimización**: Copiar el coeficiente localmente para evitar acceder a shared_data repetidamente
    double coef_local = *(data->shared->coef);

    // Calcular las nuevas temperaturas para las celdas asignadas a este hilo
    for (uint64_t i = data->start_row; i < data->end_row; i++) {
        for (uint64_t j = 1; j < data->columns - 1; j++) {
            // Usar el coeficiente local para optimizar el acceso
            double new_temp = data->local_matrix[i][j] +
                              coef_local * (data->local_matrix[i-1][j] + data->local_matrix[i+1][j] +
                                      data->local_matrix[i][j-1] + data->local_matrix[i][j+1] - 
                                      4 * data->local_matrix[i][j]);

            data->local_matrix[i][j] = new_temp;
        }
    }
    return NULL;
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
void copy_matrix(double** dest_matrix, double** src_matrix, uint64_t rows, uint64_t columns) {
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
