//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>  // Para clock_gettime
#include <mpi.h>

#include "heat_simulation.h"

/**
 * @brief Programa principal para ejecutar la simulación de transferencia de calor distribuida entre varios procesos.
 * 
 * @param argc Número de argumentos pasados a la línea de comandos.
 * @param argv Arreglo de cadenas que contiene los argumentos pasados a la línea de comandos.
 * 
 * @return 0 si la simulación se ejecuta correctamente, 1 si hay un error.
 */
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Rango del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Número total de procesos

    if (argc < 3) {
        if (rank == 0) {
            printf("Uso: %s <carpeta> <archivo de trabajo>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    const char *folder = argv[1];
    const char *jobName = argv[2];

    double **global_matrix = NULL;
    uint64_t rows = 0, columns = 0;

    // Proceso maestro: leer matriz global y obtener dimensiones
    if (rank == 0) {
        printf("Leyendo archivo de trabajo...\n");
        params_matrix *variables;
        uint64_t lines;
        variables = read_job_txt(jobName, folder, &lines);

        if (!variables) {
            fprintf(stderr, "Error al leer el archivo de trabajo.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Leer la primera matriz para determinar filas y columnas
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", folder, variables[0].filename);
        FILE *bin_file = fopen(path, "rb");
        if (!bin_file) {
            fprintf(stderr, "No se pudo abrir el archivo binario %s\n", path);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fread(&rows, sizeof(uint64_t), 1, bin_file);
        fread(&columns, sizeof(uint64_t), 1, bin_file);

        global_matrix = create_empty_matrix(rows, columns);
        for (uint64_t i = 0; i < rows; i++) {
            fread(global_matrix[i], sizeof(double), columns, bin_file);
        }
        fclose(bin_file);
    }

    // Difundir las dimensiones a todos los procesos
    MPI_Bcast(&rows, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&columns, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    // Calcular las filas locales para cada proceso
    uint64_t local_rows = rows / size + (rank < rows % size ? 1 : 0);
    uint64_t start_row = (rows / size) * rank + (rank < rows % size ? rank : rows % size);

    // Reserva memoria para la submatriz local
    double *local_matrix = malloc(local_rows * columns * sizeof(double));

    // Scatter personalizado para enviar filas a cada proceso
    int *send_counts = NULL, *displs = NULL;
    if (rank == 0) {
        send_counts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));

        for (int i = 0; i < size; i++) {
            uint64_t proc_rows = rows / size + (i < rows % size ? 1 : 0);
            send_counts[i] = proc_rows * columns;
            displs[i] = (rows / size) * i * columns + (i < rows % size ? i * columns : (rows % size) * columns);
        }
    }

    MPI_Scatterv(global_matrix ? &global_matrix[0][0] : NULL, send_counts, displs, MPI_DOUBLE,
                 local_matrix, local_rows * columns, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    // Liberar memoria de las estructuras de envío en el maestro
    if (rank == 0) {
        free(send_counts);
        free(displs);
        free_matrix(global_matrix, rows);
    }

    // Simulación de transferencia de calor local
    printf("Proceso %d realizando simulación local...\n", rank);
    uint64_t local_states = heat_transfer_simulation((double **)local_matrix, local_rows, columns, 0.01, 1.0, 1.0, 0.001);

    // Recolección de resultados en el maestro
    double *gathered_matrix = NULL;
    if (rank == 0) {
        gathered_matrix = malloc(rows * columns * sizeof(double));
    }

    MPI_Gatherv(local_matrix, local_rows * columns, MPI_DOUBLE,
                gathered_matrix, send_counts, displs, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Proceso maestro genera el reporte
    if (rank == 0) {
        printf("Generando reporte...\n");
        generate_report_file(folder, jobName, NULL, NULL, rows); // Adaptar con los resultados correctos
        free(gathered_matrix);
    }

    // Liberar memoria local
    free(local_matrix);

    MPI_Finalize();
    return 0;
}
