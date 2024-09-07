// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_GREET_LEN 256

/**
 * @brief Estructura para almacenar los datos compartidos entre los hilos.
 */
typedef struct shared_data {
  /* Arreglo de cadenas que contiene los saludos de cada hilo.*/
  char** greets;
  /* Cantidad de hilos a crear. */
  uint64_t thread_count;
} shared_data_t;

/**
 * @brief Estructura para almacenar los datos privados de cada hilo.
 */
typedef struct private_data {
  /* Número de hilo (rank) asignado a cada hilo*/
  uint64_t thread_number;
  /*Apuntador a los datos compartidos entre los hilos*/
  shared_data_t* shared_data;
} private_data_t;

/**
 * @brief Función que ejecuta cada hilo para generar un saludo.
 * 
 * @param data Apuntador a los datos privados del hilo.
 * @return NULL al terminar la ejecución del hilo.
 */
void* greet(void* data);

/**
 * @brief Crea y ejecuta los hilos de acuerdo con los datos compartidos.
 * 
 * @param shared_data Apuntador a la estructura de datos compartidos.
 * @return int Código de error. EXIT_SUCCESS si no hubo errores.
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Función principal del programa. Crea los hilos y mide el tiempo de ejecución.
 * 
 * @param argc Cantidad de argumentos recibidos.
 * @param argv Arreglo de argumentos recibidos desde la línea de comandos.
 * @return int Código de error. EXIT_SUCCESS si no hubo errores.
 */
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  /* Determina la cantidad de hilos según el argumento de la línea 
  de comandos o el número de procesadores disponibles*/
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) != 1) {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }

  // Asigna memoria para los datos compartidos
  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    shared_data->greets = (char**) calloc(thread_count, sizeof(char*));
    shared_data->thread_count = thread_count;

    if (shared_data->greets) {
      struct timespec start_time, finish_time;
      clock_gettime(CLOCK_MONOTONIC, &start_time);

      error = create_threads(shared_data);

      clock_gettime(CLOCK_MONOTONIC, &finish_time);
      double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
        (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;

      printf("Execution time: %.9lfs\n", elapsed_time);

      free(shared_data->greets);
    } else {
      fprintf(stderr, "Error: could not allocate greets\n");
      error = 13;
    }
    free(shared_data);
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    error = 12;
  }
  return error;
}

/**
 * @brief Crea los hilos de ejecución y los une al finalizar, imprimiendo los saludos generados.
 * 
 * @param shared_data Apuntador a los datos compartidos entre los hilos.
 * @return int Código de error, EXIT_SUCCESS si no hubo errores.
 */
int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  // Asigna memoria para los identificadores de hilos y los datos privados
  pthread_t* threads = (pthread_t*)malloc(shared_data->thread_count
  * sizeof(pthread_t));

  private_data_t* private_data = (private_data_t*)calloc
  (shared_data->thread_count, sizeof(private_data_t));

  if (threads && private_data) {
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      shared_data->greets[thread_number] = (char*)malloc
      (MAX_GREET_LEN * sizeof(char));
      if (shared_data->greets[thread_number]) {
        shared_data->greets[thread_number][0] = '\0';
        private_data[thread_number].thread_number = thread_number;
        private_data[thread_number].shared_data = shared_data;

        // Crea el hilo
        error = pthread_create(&threads[thread_number], NULL,
        greet, &private_data[thread_number]);
        if (error != EXIT_SUCCESS) {
          fprintf(stderr, "Error: could not create secondary thread\n");
          error = 21;
          break;
        }
      } else {
        fprintf(stderr, "Error: could not init semaphore\n");
        error = 22;
        break;
      }
    }

    // Imprime un mensaje desde el hilo principal
    printf("Hello from main thread\n");

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      pthread_join(threads[thread_number], NULL);
    }

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      printf("%s\n", shared_data->greets[thread_number]);
      free(shared_data->greets[thread_number]);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n",
    shared_data->thread_count); //NOLINT
    error = 22;
  }

  return error;
}

/**
 * @brief Función ejecutada por cada hilo, genera un saludo personalizado.
 * 
 * @param data Apuntador a los datos privados del hilo.
 * @return void* NULL al finalizar.
 */
void* greet(void* data) {
  assert(data);
  private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = private_data->shared_data;

  sprintf(shared_data->greets[private_data->thread_number], //NOLINT
    "Hello from secondary thread %" PRIu64 " of %" PRIu64,
    private_data->thread_number, shared_data->thread_count);

  return NULL;
}
