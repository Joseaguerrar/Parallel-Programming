// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief estructura que almacena los datos compartidos por todos los hilos.
 */
typedef struct shared_data {
  uint64_t next_thread;  /** Número del próximo thread a ejecutarse */
  uint64_t thread_count;  /** Número total de threads */
  uint64_t max_delay;  /** Cantidad máxima de delay indicada por el usuario */
} shared_data_t;

/**
 * @brief estructura que almacena los datos privados de los hilos
 */
typedef struct private_data {
  uint64_t thread_number;  /** rank - número respectivo del hilo */
  shared_data_t* shared_data;  /** puntero a los datos compartidos */
  unsigned int seed;           /** Semilla para rand_r */
} private_data_t;

/**
 * @brief Función que emite un saludo
 * @param data Recibe un puntero a los datos privados de cada hilo.
 */
void* greet(void* data);

/**
 * @brief Función que crea el arreglo de hilos y asigna a cada uno su información respectiva.
 * @param shared_data Puntero a la estructura de datos compartidos.
 */
int create_threads(shared_data_t* shared_data);

// procedure main(argc, argv[])
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  // create thread_count as result of converting argv[1] to integer
  // thread_count := integer(argv[1])
  // Sacamos los argumentos como siempre.

  // thread_count := integer(argv[1])
  // max_delay := integer(argv[2])
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  uint64_t max_delay = 0;
  // Verificamos que la cantidad de argumentos sea la correcta.
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Uso: %s <thread_count> [max_delay_microsec]\n", argv[0]);
    return 1;
  }

  // Parsear thread_count
  if (sscanf(argv[1], "%" SCNu64, &thread_count) != 1) {
    fprintf(stderr, "Error: thread_count inválido\n");
    return 11;
  }

  // Parsear max_delay si se proporciona
  if (argc == 3) {
    if (sscanf(argv[2], "%" SCNu64, &max_delay) == 1) {
    } else {
      fprintf(stderr, "Error: max_delay inválido\n");
      return 12;
    }
  }

  // Creamos la estructura de datos compartidos y asignamos los datos.
  shared_data_t* shared_data = <shared_data_t*>calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    shared_data->next_thread = 0;
    shared_data->thread_count = thread_count;
    shared_data->max_delay = max_delay;

    // Medimos el tiempo que tardan en ejecutarse los hilos.
    struct timespec start_time, finish_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

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
  pthread_t* threads = <pthread_t*>
  // Creamos el array de hilos
    malloc(shared_data->thread_count * sizeof(pthread_t));
  private_data_t* private_data = <private_data_t*>
  // Espacio de memoria privado para cada hilo.
    calloc(shared_data->thread_count, sizeof(private_data_t));
    // Verificamos que se crearon y pasamos por cada uno asignando.
  if (threads && private_data) {
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].shared_data = shared_data;
      private_data[thread_number].seed = time(NULL) ^ thread_number;
      // Inicializar semilla
      // create_thread(greet, thread_number)
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, greet
        , /*arg*/ &private_data[thread_number]);
        // Mandamos el hilo, la función, y los datos privados.
      if (error == EXIT_SUCCESS) {
      } else {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

    // print "Hello from main thread"
    /* Hay que mencionar que el hilo no espera a nadie, 
    por lo que no seguirá el orden de espera activa.*/
    printf("Hello from main thread\n");
    // Esperamos a cada uno de los hijos.
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
    }

    // Liberamos memoria.
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
  assert(data);
  /* Biblioteca de pruebas unitarias, nos dice si la memoria es válida o no.
  Si no fuera válida, lanza una excepción.*/ 
  private_data_t* private_data = <private_data_t*> data;  // Casteo
  // Puntero a los datos compartidos.
  shared_data_t* shared_data = private_data->shared_data;
  unsigned int nSeed = private_data->seed;
  // Wait until it is my turn
  // Espera activa. Espera hasta que el número de hilo sea el que corresponde.
  /* Esto ocurre si el número de hilo que está ejecutándose
  es mayor al que toca en ese momento*/
  // para seguir un orden.
  while (shared_data->next_thread < private_data->thread_number) {
    // busy-waiting
    // Vamos a agregar un retraso de 50 microsegundos.
    usleep(100);

    // Vamos a usar retraso pseudoaleatorio.
    // const unsigned my_delay = rand_r(&nSeed) % shared_data->max_delay;
    // usleep(my_delay);
  }  // end while

  // print "Hello from secondary thread"
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n"
    , private_data->thread_number, shared_data->thread_count);

  // Allow subsequent thread to do the task
  ++shared_data->next_thread;  // Aumentamos al siguiente número de hilo.

  return NULL;
}  // end procedure
