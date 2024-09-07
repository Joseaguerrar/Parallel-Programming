// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief Estructura que contiene los datos compartidos entre los hilos.
 * 
 * Esta estructura incluye un arreglo de semáforos para controlar el orden de saludo de cada hilo
 * y el número total de hilos.
 */
typedef struct shared_data {
  /* Arreglo de semáforos que permiten controlar
  el turno de saludo de cada hilo. */
  sem_t* can_greet;
  /* Número total de hilos. */
  uint64_t thread_count;
} shared_data_t;

/**
 * @brief Estructura que contiene los datos privados de cada hilo.
 * 
 * Esta estructura incluye el número de hilo (rank) y un puntero a los datos compartidos.
 */
typedef struct private_data {
  /* Número de hilo (rank) asignado a cada hilo. */
  uint64_t thread_number;
  /*Puntero a la estructura de datos compartidos. */
  shared_data_t* shared_data;
} private_data_t;

/**
 * @brief Función ejecutada por cada hilo para realizar el saludo.
 * 
 * @param data Puntero a los datos privados del hilo.
 * @return void* Retorna NULL al finalizar.
 * 
 * Cada hilo espera su turno para saludar usando semáforos, imprime su mensaje y luego
 * permite al siguiente hilo ejecutar.
 */
void* greet(void* data);

/**
 * @brief Crea y ejecuta los hilos que realizarán los saludos.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos entre los hilos.
 * @return int Retorna EXIT_SUCCESS si no hubo errores, o un código de error en caso contrario.
 * 
 * Esta función inicializa los hilos, les asigna su tarea y los sincroniza usando semáforos.
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Función principal del programa.
 * 
 * @param argc Número de argumentos de la línea de comandos.
 * @param argv Arreglo de argumentos de la línea de comandos.
 * @return int Retorna EXIT_SUCCESS si el programa se ejecuta correctamente, o un código de error en caso contrario.
 * 
 * La función principal inicializa la estructura de datos compartidos, crea los hilos y mide el tiempo de ejecución.
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

  // Asigna memoria para la estructura de datos compartidos
  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    // Asigna memoria para los semáforos y define la cantidad de hilos
    shared_data->can_greet = (sem_t*) calloc(thread_count, sizeof(sem_t));
    shared_data->thread_count = thread_count;

    // Inicializa los semáforos, permitiendo que solo el primer hilo comience
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
     ++thread_number) {
      error = sem_init(&shared_data->can_greet[thread_number], /*pshared*/ 0,
      /*value*/ !thread_number);
    }

    if (shared_data->can_greet) {
      struct timespec start_time, finish_time;
      clock_gettime(CLOCK_MONOTONIC, &start_time);

      // Crea y ejecuta los hilos
      error = create_threads(shared_data);

      clock_gettime(CLOCK_MONOTONIC, &finish_time);
      double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
        (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;

      printf("Execution time: %.9lfs\n", elapsed_time);

      // Libera los semáforos y la memoria de datos compartidos
      free(shared_data->can_greet);
    } else {
      fprintf(stderr, "Error: could not allocate semaphores\n");
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
 * @brief Crea los hilos y los sincroniza para que saluden en orden.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos.
 * @return int Retorna EXIT_SUCCESS si no hubo errores, o un código de error si falló.
 * 
 * Esta función inicializa y lanza todos los hilos. Los hilos se sincronizan mediante semáforos
 * para saludar en el orden correcto.
 */
int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;

  // Asigna memoria para los hilos y sus datos privados
  pthread_t* threads = (pthread_t*)malloc(shared_data->thread_count *
  sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*)calloc
  (shared_data->thread_count, sizeof(private_data_t));

  if (threads && private_data) {
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].shared_data = shared_data;

      // Crea el hilo
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, greet,
      /*arg*/ &private_data[thread_number]);
      if (error != EXIT_SUCCESS) {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

    // Imprime un mensaje desde el hilo principal
    printf("Hello from main thread\n");

    // Espera a que todos los hilos terminen y destruye los semáforos
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
      sem_destroy(&shared_data->can_greet[thread_number]);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n",
    shared_data->thread_count);
    error = 23;
  }

  return error;
}

/**
 * @brief Función ejecutada por cada hilo para realizar el saludo en orden.
 * 
 * @param data Puntero a la estructura privada del hilo.
 * @return void* Retorna NULL al finalizar.
 * 
 * Cada hilo espera su turno utilizando un semáforo, imprime su saludo, y luego
 * activa el siguiente hilo.
 */
void* greet(void* data) {
  assert(data);
  private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = private_data->shared_data;

  // Espera su turno
  int error = sem_wait(&shared_data->can_greet[private_data->thread_number]);
  if (error) {
    fprintf(stderr, "error: could not wait for semaphore\n");
  }

  // Imprime el saludo del hilo
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n",
         private_data->thread_number, shared_data->thread_count);

  // Permite al siguiente hilo saludar
  const uint64_t next_thread = (private_data->thread_number + 1) %
  shared_data->thread_count;
  error = sem_post(&shared_data->can_greet[next_thread]);
  if (error) {
    fprintf(stderr, "error: could not increment semaphore\n");
  }

  return NULL;
}
