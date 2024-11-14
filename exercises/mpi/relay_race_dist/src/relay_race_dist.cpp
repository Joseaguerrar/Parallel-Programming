//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>

/**
 * @file relay_race.cpp
 * @brief Simula una carrera de relevos utilizando procesos MPI.
 * 
 * Este programa utiliza MPI para simular una carrera de relevos con múltiples equipos.
 * Cada equipo tiene dos etapas. Un proceso árbitro supervisa la carrera
 * y reporta los resultados.
 */

#include <mpi.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define fail(msg) throw std::runtime_error(msg)

/**
 * @brief Simula la carrera de relevos.
 * 
 * @param argc Número de argumentos de la línea de comandos.
 * @param argv Array de argumentos de la línea de comandos.
 * @param process_number El número de rango del proceso MPI actual.
 * @param process_count El número total de procesos MPI.
 */
void simulate_relay_race(int argc, char* argv[], int process_number,
                                                             int process_count);

/**
 * @brief Ejecuta la primera etapa de la carrera para un equipo.
 * 
 * @param stage1_delay Retraso en milisegundos para la etapa 1.
 * @param process_number El número de rango del proceso MPI actual.
 * @param team_count El número total de equipos.
 */
void run_stage1(int stage1_delay, int process_number, int team_count);

/**
 * @brief Ejecuta la segunda etapa de la carrera para un equipo.
 * 
 * @param stage2_delay Retraso en milisegundos para la etapa 2.
 * @param process_number El número de rango del proceso MPI actual.
 * @param team_count El número total de equipos.
 */
void run_stage2(int stage2_delay, int process_number, int team_count);

/**
 * @brief Supervisa la carrera y reporta los resultados.
 * 
 * @param team_count El número total de equipos.
 */
void referee(int team_count);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    try {
      int process_number = -1;
      MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

      int process_count = -1;
      MPI_Comm_size(MPI_COMM_WORLD, &process_count);

      char process_hostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
      int hostname_length = -1;
      MPI_Get_processor_name(process_hostname, &hostname_length);

      simulate_relay_race(argc, argv, process_number, process_count);
    } catch (const std::runtime_error& exception) {
      std::cout << exception.what() << std::endl;
      error = EXIT_FAILURE;
    }
    MPI_Finalize();
  } else {
    std::cerr << "error: no se pudo inicializar MPI" << std::endl;
    error = EXIT_FAILURE;
  }
  return error;
}

void simulate_relay_race(int argc, char* argv[], int process_number,
                                                            int process_count) {
  if (argc == 3) {
    if (process_count >= 3 && process_count % 2 == 1) {
      const int team_count = (process_count - 1) / 2;
      const int stage1_delay = atoi(argv[1]);
      const int stage2_delay = atoi(argv[2]);

      if (process_number == 0) {
        referee(team_count);
      } else if (process_number <= team_count) {
        run_stage1(stage1_delay, process_number, team_count);
      } else {
        run_stage2(stage2_delay, process_number, team_count);
      }
    } else {
      fail("error: el número de procesos debe ser impar y mayor a 3");
    }
  } else {
    fail("uso: relay_race_dist stage1_delay stage2_delay");
  }
}

void run_stage1(int stage1_delay, int process_number, int team_count) {
  // Esperar en la barrera
  if (MPI_Barrier(MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: no se pudo esperar en la barrera");
  }
  usleep(1000 * stage1_delay);
  const int peer = process_number + team_count;
  bool baton = true;
  // Enviar testigo al compañero
  if (MPI_Send(&baton, /*count*/ 1, MPI_C_BOOL, peer, /*tag*/ 0, MPI_COMM_WORLD)
                                                               != MPI_SUCCESS) {
    fail("error: no se pudo enviar el testigo");
  }
}

void run_stage2(int stage2_delay, int process_number, int team_count) {
  // Esperar en la barrera
  if (MPI_Barrier(MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: no se pudo esperar en la barrera");
  }
  int peer = process_number - team_count;
  bool baton = false;
  // Recibir testigo del compañero
  if (MPI_Recv(&baton, /*capacity*/ 1, MPI_C_BOOL, /*source*/ peer, /*tag*/ 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
    fail("error: no se pudo recibir el testigo");
  }
  usleep(1000 * stage2_delay);
  // Enviar el número del equipo al árbitro
  if (MPI_Send(&peer, /*count*/ 1, MPI_INT, /*target*/ 0, /*tag*/ 0,
                                               MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: no se pudo enviar el número del equipo al árbitro");
  }
}

void referee(int team_count) {
  const double start_time = MPI_Wtime();
  // Esperar en la barrera
  if (MPI_Barrier(MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: no se pudo esperar en la barrera");
  }
  int place = 0;
  for (int index = 0; index < team_count; ++index) {
    int team = 0;
    // Recibir el número del equipo de cualquier proceso
    if (MPI_Recv(&team, /*capacity*/ 1, MPI_INT, MPI_ANY_SOURCE,
                 /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
      fail("error: no se pudo recibir el número del equipo");
    }
    const double elapsed = MPI_Wtime() - start_time;
    ++place;
    std::cout << "Lugar " << place << ": equipo " << team << " en " <<
                                                    elapsed << "s" << std::endl;
  }
}
