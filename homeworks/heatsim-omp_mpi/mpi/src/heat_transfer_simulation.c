//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "heat_simulation.h"

void read_bin_plate(const char* folder, params_matrix* variables, uint64_t lines, const char* jobName) {
    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    uint64_t rows, columns;
    char direction[1024]; // Incrementar el tamaño para rutas largas
    uint64_t* array_state_k = malloc(lines * sizeof(uint64_t));

    for (uint64_t i = 0; i < lines; i++) {
        // Validar que variables[i].filename no sea NULL
        if (variables[i].filename == NULL) {
            fprintf(stderr, "Proceso %d: Filename es NULL para la simulación %lu\n", rank, i);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Generar la ruta completa y validar que no se trunque
        int ret = snprintf(direction, sizeof(direction), "%s/%s", folder, variables[i].filename);
        if (ret < 0 || ret >= sizeof(direction)) {
            fprintf(stderr, "Proceso %d: La ruta generada es demasiado larga para el buffer.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Abrir el archivo binario y validar
        FILE* bin_file = fopen(direction, "rb");
        if (bin_file == NULL) {
            fprintf(stderr, "Proceso %d: No se pudo abrir el archivo binario %s\n", rank, variables[i].filename);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Leer dimensiones de la matriz
        fread(&rows, sizeof(uint64_t), 1, bin_file);
        fread(&columns, sizeof(uint64_t), 1, bin_file);

        // Asignar memoria para la matriz local
        double** matrix = create_empty_matrix(rows, columns);
        if (matrix == NULL) {
            fprintf(stderr, "Proceso %d: Error al asignar memoria para la matriz\n", rank);
            fclose(bin_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Leer los datos de la matriz
        for (uint64_t i = 0; i < rows; i++) {
            fread(matrix[i], sizeof(double), columns, bin_file);
        }
        fclose(bin_file);

        // Simulación de transferencia de calor
        double delta_t = variables[i].delta_t;
        double alpha = variables[i].alpha;
        double h = variables[i].h;
        double epsilon = variables[i].epsilon;

        uint64_t states_k = heat_transfer_simulation(matrix, rows, columns, delta_t, alpha, h, epsilon);
        array_state_k[i] = states_k;

        // Generar el archivo binario con el estado final
        generate_bin_file(matrix, rows, columns, folder, variables[i].filename, states_k);

        // Liberar la memoria de la matriz
        free_matrix(matrix, rows);
    }

    // Generar el archivo de reporte en el proceso raíz
    if (rank == 0) {
        generate_report_file(folder, jobName, variables, array_state_k, lines);
    }

    free(array_state_k);
}

uint64_t heat_transfer_simulation(double** matrix, uint64_t rows, uint64_t columns,
                                      double delta_t, double alpha, double h, double epsilon) {
    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Calcular el rango de filas para este proceso
    uint64_t local_rows = rows / world_size + (rank < rows % world_size ? 1 : 0);
    uint64_t start_row = rank * (rows / world_size) + (rank < rows % world_size ? rank : rows % world_size);

    // Crear matrices locales
    double** current_matrix = create_empty_matrix(local_rows + 2, columns); // Incluye filas fantasma
    double** next_matrix = create_empty_matrix(local_rows + 2, columns);

    // Copiar las filas correspondientes de la matriz global
    for (uint64_t i = 0; i < local_rows; i++) {
        memcpy(current_matrix[i + 1], matrix[start_row + i], columns * sizeof(double));
    }

    uint64_t states_k = 0;
    bool balance_point = false;

    while (!balance_point) {
        balance_point = true;

        // Intercambiar filas frontera con procesos vecinos
        if (rank > 0) {
            MPI_Sendrecv(current_matrix[1], columns, MPI_DOUBLE, rank - 1, 0,
                         current_matrix[0], columns, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < world_size - 1) {
            MPI_Sendrecv(current_matrix[local_rows], columns, MPI_DOUBLE, rank + 1, 0,
                         current_matrix[local_rows + 1], columns, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Actualizar las celdas internas
        for (uint64_t i = 1; i <= local_rows; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                next_matrix[i][j] = current_matrix[i][j] +
                    ((delta_t * alpha) / (h * h)) * (current_matrix[i - 1][j] +
                                                     current_matrix[i + 1][j] +
                                                     current_matrix[i][j - 1] +
                                                     current_matrix[i][j + 1] -
                                                     4 * current_matrix[i][j]);
                if (fabs(next_matrix[i][j] - current_matrix[i][j]) > epsilon) {
                    balance_point = false;
                }
            }
        }

        // Sincronización para evaluar el balance
        bool global_balance_point;
        MPI_Allreduce(&balance_point, &global_balance_point, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        balance_point = global_balance_point;

        // Intercambiar matrices
        double** temp = current_matrix;
        current_matrix = next_matrix;
        next_matrix = temp;

        states_k++;
    }

    // Enviar datos al proceso raíz
    if (rank == 0) {
        for (uint64_t i = 0; i < local_rows; i++) {
            memcpy(matrix[start_row + i], current_matrix[i + 1], columns * sizeof(double));
        }
    } else {
        for (uint64_t i = 0; i < local_rows; i++) {
            MPI_Send(current_matrix[i + 1], columns, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }

    // Liberar memoria
    free_matrix(current_matrix, local_rows + 2);
    free_matrix(next_matrix, local_rows + 2);

    return states_k;
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
