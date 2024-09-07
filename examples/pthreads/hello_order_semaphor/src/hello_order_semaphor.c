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
 * Esta estructura incluye un arreglo de semáforos para controlar cuándo cada hilo puede saludar
 * y el número total de hilos creados.
 */
typedef struct shared_data {
  /* Arreglo de semáforos que permiten controlar 
  el turno de saludo de cada hilo. */
  sem_t* can_greet;
  /* Número total de hilos a crear. */
  uint64_t thread_count;
} shared_data_t;

/**
 * @brief Estructura que contiene los datos privados de cada hilo.
 * 
 * Esta estructura incluye el número de hilo (rank) y un puntero a los datos compartidos.
 */
typedef struct private_data {
  uint64_t thread_number; /* Número de hilo (rank) asignado a cada hilo. */
  shared_data_t* shared_data; /*Puntero a la estructura de datos compartidos. */
} private_data_t;

/**
 * @brief Función ejecutada por cada hilo para realizar el saludo.
 * 
 * @param data Puntero a los datos privados del hilo.
 * @return void* Retorna NULL al finalizar.
 * 
 * Esta función espera su turno (controlado por semáforos) y luego imprime un saludo.
 */
void* greet(void* data);

/**
 * @brief Crea los hilos y les asigna la tarea de saludar.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos entre los hilos.
 * @return int Retorna EXIT_SUCCESS si no hubo errores, o un código de error en caso contrario.
 * 
 * Esta función inicializa los hilos, ejecuta la tarea de saludo y luego espera a que todos terminen.
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Función principal del programa.
 * 
 * @param argc Número de argumentos de la línea de comandos.
 * @param argv Arreglo de argumentos de la línea de comandos.
 * @return int Retorna EXIT_SUCCESS si el programa se ejecuta correctamente, o un código de error en caso contrario.
 * 
 * Esta función inicializa los semáforos y los hilos, mide el tiempo de ejecución y libera los recursos al final.
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

    // Inicializa los semáforos para cada hilo
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      /* Inicializa el semáforo de cada hilo 
      con valor 0 excepto para el primer hilo*/
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
}  // end procedure


int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  // for thread_number := 0 to thread_count do
  pthread_t* threads = (pthread_t*)
    malloc(shared_data->thread_count * sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*)
    calloc(shared_data->thread_count, sizeof(private_data_t));
  if (threads && private_data) {
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      if (error == EXIT_SUCCESS) {
        private_data[thread_number].thread_number = thread_number;
        private_data[thread_number].shared_data = shared_data;
        // create_thread(greet, thread_number)
        error = pthread_create(&threads[thread_number], /*attr*/ NULL, greet
          , /*arg*/ &private_data[thread_number]);
        if (error == EXIT_SUCCESS) {
        } else {
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

    // print "Hello from main thread"
    printf("Hello from main thread\n");

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
      sem_destroy(&shared_data->can_greet[thread_number]);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n"
      , shared_data->thread_count);
    error = 23;
  }

  return error;
}

// procedure greet:
void* greet(void* data) {
  assert(data);
  private_data_t* private_data = (private_data_t*) data;
  shared_data_t* shared_data = private_data->shared_data;

  // Wait until it is my turn
  // wait(can_greet[thread_number])
  int error = sem_wait(&shared_data->can_greet[private_data->thread_number]);
  if (error) {
    fprintf(stderr, "error: could not wait for semaphore\n");
  }

  // print "Hello from secondary thread"
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n"
    , private_data->thread_number, shared_data->thread_count);

  // Allow subsequent thread to do the task
  // signal(can_greet[(thread_number + 1) mod thread_count])
  const uint64_t next_thread = (private_data->thread_number + 1)
    % shared_data->thread_count;

  error = sem_post(&shared_data->can_greet[next_thread]);
  if (error) {
    fprintf(stderr, "error: could not increment semaphore\n");
  }

  return NULL;
}  // end procedure
