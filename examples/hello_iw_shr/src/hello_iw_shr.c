// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/time.h>//se incluyó porque me tiraba error sin esta

/**
 * @file hello_iw_shr.c
 * @brief Ejemplo de un programa que crea múltiples hilos y mide el tiempo de ejecución.
 *
 * Este archivo contiene un programa en C que utiliza hilos (threads) para 
 * ejecutar tareas en paralelo. El programa define dos estructuras de datos: 
 * `shared_data_t` que contiene datos compartidos por todos los hilos, 
 * y `private_data_t` que contiene datos privados específicos para cada hilo.
 */

/**
 * @brief Estructura que contiene datos compartidos por todos los hilos.
 *
 * Esta estructura incluye un contador de hilos (`thread_count`) que se utiliza
 * para determinar cuántos hilos se deben crear.
 */
typedef struct shared_data {
  uint64_t thread_count;  ///< Número de hilos que se deben crear.
} shared_data_t;

/**
 * @brief Estructura que contiene datos privados para cada hilo.
 *
 * Cada hilo tiene un número de hilo (`thread_number`) único y un puntero a 
 * la estructura de datos compartidos (`shared_data`).
 */
typedef struct private_data {
  uint64_t thread_number;  ///< Número de hilo (identificador único del hilo).
  shared_data_t* shared_data;  ///< Puntero a la estructura de datos compartidos.
} private_data_t;

/**
 * @brief Función que ejecuta cada hilo.
 *
 * Esta función es llamada por cada hilo creado y se encarga de imprimir un 
 * mensaje indicando cuál hilo se está ejecutando.
 *
 * @param data Puntero a los datos privados del hilo (`private_data_t`).
 * @return Siempre retorna NULL.
 */
void* greet(void* data);

/**
 * @brief Crea los hilos y espera a que todos terminen.
 *
 * Esta función se encarga de crear el número de hilos especificado en 
 * `shared_data_t` y luego esperar a que todos los hilos terminen su ejecución.
 *
 * @param shared_data Puntero a la estructura que contiene los datos compartidos por todos los hilos.
 * @return Un código de error si no se pudieron crear los hilos, de lo contrario EXIT_SUCCESS.
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Punto de entrada del programa.
 *
 * La función principal realiza las siguientes tareas:
 * - Inicializa el número de hilos a crear, basándose en los argumentos de línea de comandos
 *   o en el número de procesadores disponibles.
 * - Asigna memoria para los datos compartidos entre los hilos.
 * - Mide el tiempo de ejecución del programa desde el inicio hasta la finalización de todos los hilos.
 * - Libera la memoria asignada y maneja errores de asignación de memoria o creación de hilos.
 *
 * @param argc Número de argumentos pasados al programa. Se espera que argc sea 2 si se especifica el número de hilos.
 * @param argv Vector de cadenas con los argumentos pasados al programa. `argv[1]` debería contener el número de hilos a crear.
 * @return EXIT_SUCCESS si todo salió bien, o un código de error si ocurrió algún problema.
 */
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  
  // Obtiene el número de hilos basado en el número de procesadores disponibles.
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  
  // Si se pasa un argumento en la línea de comandos, lo convierte a un número entero.
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) == 1) {
      // thread_count se ha actualizado correctamente.
    } else {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }

  // Asigna memoria para los datos compartidos por todos los hilos.
  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    shared_data->thread_count = thread_count;

    // Variables para medir el tiempo de ejecución.
    struct timespec start_time, finish_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Crea los hilos.
    error = create_threads(shared_data);

    // Mide el tiempo de ejecución.
    clock_gettime(CLOCK_MONOTONIC, &finish_time);
    double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
      (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;

    printf("Execution time: %.9lfs\n", elapsed_time);

    // Libera la memoria asignada para los datos compartidos.
    free(shared_data);
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    return 12;
  }
  return error;
}  // end procedure main

/**
 * @brief Crea los hilos especificados y espera a que terminen.
 *
 * Esta función reserva memoria para los hilos y sus datos privados, luego 
 * crea cada hilo, asignándole sus datos privados y compartidos. Finalmente, 
 * espera a que todos los hilos terminen su ejecución antes de liberar la 
 * memoria reservada.
 *
 * @param shared_data Puntero a la estructura que contiene los datos compartidos entre los hilos.
 * @return EXIT_SUCCESS si todo salió bien, o un código de error si hubo problemas al crear los hilos.
 */
int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;

  // Reserva memoria para los identificadores de los hilos y sus datos privados.
  pthread_t* threads = (pthread_t*)
    malloc(shared_data->thread_count * sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*)
    calloc(shared_data->thread_count, sizeof(private_data_t));
  
  if (threads && private_data) {
    // Crea cada hilo y le asigna sus datos privados y compartidos.
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].shared_data = shared_data;

      // Crea el hilo y le pasa la función greet para que la ejecute.
      error = pthread_create(&threads[thread_number], NULL, greet, &private_data[thread_number]);
      if (error != EXIT_SUCCESS) {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

    // Imprime un mensaje desde el hilo principal.
    printf("Hello from main thread\n");

    // Espera a que todos los hilos terminen.
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count; ++thread_number) {
      pthread_join(threads[thread_number], NULL);
    }

    // Libera la memoria asignada para los hilos y sus datos.
    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n", shared_data->thread_count);
    error = 22;
  }

  return error;
}

/**
 * @brief Función que ejecuta cada hilo creado.
 *
 * Cada hilo ejecuta esta función, que recibe un puntero a su estructura de 
 * datos privados (`private_data_t`). La función imprime un mensaje indicando 
 * su número de hilo y el total de hilos creados.
 *
 * @param data Puntero a los datos privados del hilo (`private_data_t`).
 * @return Siempre retorna NULL.
 */
void* greet(void* data) {
  // Asegura que el puntero de datos no sea NULL.
  private_data_t* private_data = (private_data_t*) data;
  shared_data_t* shared_data = private_data->shared_data;

  // Imprime un mensaje indicando el número de hilo y el total de hilos.
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n",
    private_data->thread_number, shared_data->thread_count);

  return NULL;
}  // end procedure greet