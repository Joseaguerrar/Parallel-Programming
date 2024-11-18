//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>  // Para clock_gettime
#include <mpi.h>
#include "heat_simulation.h"

/**
 * @file main.c
 * @brief Programa principal para la simulación distribuida utilizando MPI.
 *
 * Este programa distribuye el trabajo de simulación térmica entre varios
 * procesos utilizando MPI, realiza las simulaciones y mide el tiempo de ejecución.
 */
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);  // Inicializar MPI

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Obtener el rango del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Obtener el número total de procesos

    if (argc < 3) {
        if (rank == 0) {
            printf("Uso: %s <carpeta> <archivo de trabajo>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    const char *folder = argv[1];
    const char *jobName = argv[2];

    struct timespec start_time, finish_time;
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);  // Medir tiempo de inicio
    }

    // Proceso raíz (rank 0) lee el archivo de trabajo
    uint64_t lines;
    params_matrix* variables = NULL;
    if (rank == 0) {
        variables = read_job_txt(jobName, folder, &lines);
        if (!variables) {
            fprintf(stderr, "Error al leer el archivo de trabajo.\n");
            MPI_Finalize();
            return 1;
        }
    }

    // Broadcast del número de simulaciones
    MPI_Bcast(&lines, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    // Distribuir trabajo entre procesos
    uint64_t rows_per_proc = lines / world_size;
    uint64_t extra_rows = lines % world_size;
    uint64_t local_start = rank * rows_per_proc + (rank < extra_rows ?
                                                             rank : extra_rows);
    uint64_t local_lines = rows_per_proc + (rank < extra_rows ? 1 : 0);

    params_matrix* local_variables = malloc(local_lines *
                                                         sizeof(params_matrix));

    // Scatter los parámetros de las simulaciones
    if (rank == 0) {
        for (int i = 0; i < world_size; i++) {
            uint64_t start = i * rows_per_proc + (i < extra_rows ? i :
                                                                    extra_rows);
            uint64_t count = rows_per_proc + (i < extra_rows ? 1 : 0);
            if (i == 0) {
                memcpy(local_variables, variables + start, count *
                                                         sizeof(params_matrix));
            } else {
                MPI_Send(variables + start, count * sizeof(params_matrix),
                                                MPI_BYTE, i, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Recv(local_variables, local_lines * sizeof(params_matrix),
                             MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Cada proceso realiza su porción de simulación
    read_bin_plate(folder, local_variables, local_lines, jobName);

    // Proceso raíz mide el tiempo de finalización
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &finish_time);
        double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
                         (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
        printf("Tiempo de ejecución: %.9lfs\n", elapsed);
    }

    // Liberar memoria
    free(local_variables);
    if (rank == 0) {
        for (uint64_t i = 0; i < lines; i++) {
            free(variables[i].filename);
        }
        free(variables);
    }

    MPI_Finalize();  // Finalizar MPI
    printf("Proceso %d: Simulación completada.\n", rank);
    return 0;
}
