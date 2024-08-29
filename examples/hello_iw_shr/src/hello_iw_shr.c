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
 * La función principal del programa. Inicializa los datos compartidos, 
 * crea los hilos y mide el tiempo de ejecución del programa.
 *
 * @param argc Número de argumentos pasados al programa.
 * @param argv Vector de cadenas con los argumentos pasados al programa.
 * @return EXIT_SUCCESS si todo salió bien, o un código de error si ocurrió algún problema.
 */
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  // create thread_count as result of converting argv[1] to integer
  // thread_count := integer(argv[1])
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) == 1) {
    } else {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }

  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
                                                    //1 porque todos comparten la memoria
  if (shared_data) {
    shared_data->thread_count = thread_count;

    struct timespec start_time, finish_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    //esto sirve para medir el tiempo

    error = create_threads(shared_data);

    clock_gettime(CLOCK_MONOTONIC, &finish_time);
    double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
      (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;

    printf("Execution time: %.9lfs\n", elapsed_time);

    free(shared_data);
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    return 12;
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
      private_data[thread_number].thread_number = thread_number;
      //tienen su espacio privado
      private_data[thread_number].shared_data = shared_data;
      //pero tambien tiene su espacio compartido
      // create_thread(greet, thread_number)
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, greet
        , /*arg*/ &private_data[thread_number]);
      if (error == EXIT_SUCCESS) {
      } else {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

    // print "Hello from main thread"
    printf("Hello from main thread\n");

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n"
      , shared_data->thread_count);
    error = 22;
  }

  return error;
}

// procedure greet:
void* greet(void* data) {
  // assert(data);
  private_data_t* private_data = (private_data_t*) data;
  shared_data_t* shared_data = private_data->shared_data;

  // print "Hello from secondary thread"
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n"
    , private_data->thread_number, shared_data->thread_count);
    //soy el hilo tal, de su memoria privada     //de tantos, de su memoria compartida
  return NULL;
}  // end procedure