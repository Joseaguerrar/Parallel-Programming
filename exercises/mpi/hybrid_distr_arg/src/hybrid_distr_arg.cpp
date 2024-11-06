//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>

#include <mpi.h>
#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>

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
 * su subrango.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Array de argumentos de línea de comandos. Se esperan dos argumentos:
 *        el inicio (inclusive) y el fin (no inclusive) del rango general.
 * @return 0 si el programa finaliza correctamente.
 */
int main(int argc, char* argv[]) {
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    int process_number = -1;  // rank
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

    int process_count = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    char process_hostname[MPI_MAX_PROCESSOR_NAME];
    int hostname_length = -1;
    MPI_Get_processor_name(process_hostname, &hostname_length);

    if (argc == 3) {
      const int overall_start = atoi(argv[1]);
      const int overall_finish = atoi(argv[2]);

      const int process_start = calculate_start(process_number, overall_finish,
                                                process_count, overall_start);
      const int process_finish = calculate_finish(process_number,
                                  overall_finish, process_count, overall_start);
      const int process_size = process_finish - process_start;

      // Informar el rango asignado al proceso
      std::cout << process_hostname << ':' << process_number << ": range ["
                << process_start << ", " << process_finish << "[ size "
                << process_size
                << std::endl;

      #pragma omp parallel
      {
        int thread_id = omp_get_thread_num();
        int thread_count = omp_get_num_threads();

        // Calcular el rango para cada hilo
        int thread_start = calculate_start(thread_id, process_finish,
                                           thread_count, process_start);
        int thread_finish = calculate_finish(thread_id, process_finish,
                                             thread_count, process_start);
        int thread_size = thread_finish - thread_start;

        #pragma omp critical
        std::cout << '\t' << process_hostname << ':' << process_number << '.'
                  << thread_id << ": range [" << thread_start << ", "
                  << thread_finish
                  << "[ size " << thread_size << std::endl;
      }
    } else {
      std::cerr << "usage: hybrid_distr_arg start finish" << std::endl;
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
