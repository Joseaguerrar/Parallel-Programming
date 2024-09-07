// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// Simulates a producer and a consumer that share a unbounded buffer

#include <stdlib.h>
#include <stdio.h>

#include "simulation.h"

/**
 * @brief Función principal que coordina la creación, ejecución y destrucción de la simulación.
 * 
 * La función principal crea una nueva simulación, ejecuta el proceso de simulación con los
 * argumentos proporcionados y, finalmente, libera los recursos de la simulación antes de finalizar.
 * 
 * @param argc Número de argumentos proporcionados en la línea de comandos.
 * @param argv Array de cadenas que contiene los argumentos de la línea de comandos.
 * @return int Devuelve EXIT_SUCCESS si la simulación se ejecuta correctamente, de lo contrario devuelve EXIT_FAILURE.
 */
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  // Crear la simulación
  simulation_t* simulation = simulation_create();
  
  if (simulation) {
    // Ejecutar la simulación
    error = simulation_run(simulation, argc, argv);
    
    // Destruir la simulación y liberar recursos
    simulation_destroy(simulation);
  } else {
    fprintf(stderr, "error: no se pudo asignar la simulación\n");
    error = EXIT_FAILURE;
  }

  return error;
}