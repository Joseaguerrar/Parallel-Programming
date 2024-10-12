// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <iostream>
#include <omp.h>

/**
 * @brief Programa que distribuye las iteraciones de un ciclo for entre varios hilos
 *        usando OpenMP y diferentes tipos de mapeo para paralelismo.
 * 
 * Este programa acepta dos parámetros por línea de comandos:
 * 1. thread_count: el número de hilos a utilizar.
 * 2. iteration_count: el número de iteraciones a realizar.
 * 
 * Si no se especifican los parámetros, se usan valores predeterminados: 
 * 1 hilo y número de iteraciones igual al número de hilos.
 * 
 * El programa emplea diferentes algoritmos de mapeo de iteraciones: 
 * - Estático por bloque (static).
 * - Estático cíclico (static con chunk).
 * - Dinámico (dynamic).
 * - Guiado (guided).
 * 
 * Cada iteración imprime el hilo que la ejecuta y el número de iteración.
 * 
 * @param argc Número de argumentos pasados por línea de comandos.
 * @param argv Vector de argumentos de tipo char*.
 * @return int Código de salida del programa.
 */
int main(int argc, char* argv[]) {
  // Número de hilos a utilizar (por defecto 1 si no se especifica).
  const int thread_count = argc >= 2 ? ::atoi(argv[1]) : 1;
  // Número de iteraciones (por defecto igual al número de hilos).
  const int iteration_count = argc >= 3 ? ::atoi(argv[2]) : thread_count;

  /**
   * @brief Ejecución paralela con reparto de iteraciones usando programación
   *        estática por bloque.
   * 
   * Cada hilo recibe un bloque fijo de iteraciones. El número de hilos está
   * definido por la variable `thread_count`, y la estrategia de mapeo está
   * indicada por `schedule(static)`.
   */
  #pragma omp parallel for num_threads(thread_count) schedule(static)
  for (int iteration = 0; iteration < iteration_count; ++iteration) {
    std::cout << "Thread " << omp_get_thread_num() << '/' << thread_count
              << ": iteration " << iteration << '/' << iteration_count << std::endl;
  }

  return 0;
}

/**
 * @brief Otras estrategias de mapeo que podrían reemplazar la anterior.
 * 
 * Estático por bloque
 * @code
 * #pragma omp parallel for num_threads(thread_count) schedule(static,chunk_size)
 * @endcode
 * 
 * Cíclico estático:
 * @code
 * #pragma omp parallel for num_threads(thread_count) schedule(static, chunk_size)
 * @endcode
 * 
 * Dinámico:
 * @code
 * #pragma omp parallel for num_threads(thread_count) schedule(dynamic, chunk_size)
 * @endcode
 * 
 * Guiado:
 * @code
 * #pragma omp parallel for num_threads(thread_count) schedule(guided, chunk_size)
 * @endcode
 * 
 */
