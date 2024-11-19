//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

/**
 * @brief Calcula el inicio de un rango asignado a un proceso o hilo.
 *
 * @param rank Identificador del proceso o hilo dentro de su grupo.
 * @param end Valor final del rango general (no incluido).
 * @param workers Número total de procesos o hilos en el grupo.
 * @param begin Valor inicial del rango general.
 * @return El valor de inicio del subrango asignado al proceso o hilo.
 */
int calculate_start(int rank, int end, int workers, int begin);

/**
 * @brief Calcula el final de un rango asignado a un proceso o hilo.
 *
 * @param rank Identificador del proceso o hilo dentro de su grupo.
 * @param end Valor final del rango general (no incluido).
 * @param workers Número total de procesos o hilos en el grupo.
 * @param begin Valor inicial del rango general.
 * @return El valor final (no incluido) del subrango asignado al proceso o hilo.
 */
int calculate_finish(int rank, int end, int workers, int begin);

/**
 * @brief Programa principal que distribuye un rango entre procesos MPI y hilos
 *        de OpenMP.
 *
 * Este programa inicializa MPI y distribuye un rango general entre los
 * diferentes procesos MPI y, dentro de cada proceso, entre los hilos de OpenMP.
 * Cada proceso informa su rango asignado, y cada hilo dentro del proceso informa
 * su subrango. El tiempo de ejecución de cada proceso es medido desde la inicialización
 * hasta la finalización de MPI.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Array de argumentos de línea de comandos. Se esperan dos argumentos:
 *        el inicio (inclusive) y el fin (no inclusive) del rango general.
 * @return 0 si el programa finaliza correctamente.
 */
int main(int argc, char* argv[]) {
    double start_time = 0.0, end_time = 0.0;

    if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
        start_time = MPI_Wtime();  ///< Tiempo inicial del proceso MPI.

        int process_number = -1;  ///< Identificador del proceso actual (rank).
        MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

        int process_count = -1;  ///< Número total de procesos en MPI.
        MPI_Comm_size(MPI_COMM_WORLD, &process_count);

        char process_hostname[MPI_MAX_PROCESSOR_NAME];
        int hostname_length = -1;
        MPI_Get_processor_name(process_hostname, &hostname_length);

        int overall_start = 0;
        int overall_finish = 0;

        if (process_number == 0) {
            // Proceso raíz recibe los límites del rango
            if (argc == 3) {
                overall_start = atoi(argv[1]);
                overall_finish = atoi(argv[2]);
            } else {
                std::cerr
                         << "usage: hybrid_distr_arg start finish" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }
        }

        // Difundir el rango general a todos los procesos
        MPI_Bcast(&overall_start, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&overall_finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Calcular subrangos para cada proceso
        std::vector<int> starts(process_count);
        std::vector<int> finishes(process_count);

        if (process_number == 0) {
            for (int i = 0; i < process_count; ++i) {
                starts[i] = calculate_start(i, overall_finish, process_count,
                                                                 overall_start);
                finishes[i] = calculate_finish(i, overall_finish, process_count,
                                                                 overall_start);
            }
        }

        // Subrango asignado a este proceso
        int process_start = 0;
        int process_finish = 0;

        MPI_Scatter(starts.data(), 1, MPI_INT, &process_start, 1, MPI_INT, 0,
                                                                MPI_COMM_WORLD);
        MPI_Scatter(finishes.data(), 1, MPI_INT, &process_finish, 1, MPI_INT, 0,
                                                                MPI_COMM_WORLD);

        int process_size = process_finish - process_start;

        // Informar el rango asignado al proceso
        std::cout << process_hostname << ':' << process_number << ": range ["
                  << process_start << ", " << process_finish << "[ size "
                  << process_size;

        end_time = MPI_Wtime();  ///< Tiempo final del proceso MPI.

        // Calcular el tiempo total del proceso
        double elapsed_time = end_time - start_time;

        // Imprimir el tiempo inmediatamente después de la información del rango
        std::cout << " in " << elapsed_time << "s" << std::endl;

        #pragma omp parallel
        {
            int thread_id = omp_get_thread_num();
            int thread_count = omp_get_num_threads();

            // Calcular el rango para cada hilo dentro del rango del proceso
            int thread_start = process_start + calculate_start(thread_id,
                                                 process_finish - process_start,
                                                            thread_count, 0);
            int thread_finish = process_start + calculate_finish(thread_id,
                                                 process_finish - process_start,
                                                               thread_count, 0);
            int thread_size = thread_finish - thread_start;

            #pragma omp critical
            std::cout << '\t' << process_hostname << ':' << process_number <<'.'
                    << thread_id << ": range [" << thread_start << ", "
                    << thread_finish
                    << "[ size " << thread_size << std::endl;
        }

        MPI_Finalize();
    }

    return 0;
}

int calculate_start(int rank, int end, int workers, int begin) {
    return rank * ((end - begin) / workers) +
           std::min(rank, (end - begin) % workers);
}

int calculate_finish(int rank, int end, int workers, int begin) {
    return calculate_start(rank + 1, end, workers, begin);
}
