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
 * @brief Estructura que almacena los datos compartidos entre los hilos.
 * 
 * Esta estructura contiene el número de hilos, la cantidad de hits disponibles
 * para todos los hilos, y una posición que se incrementa de manera concurrente.
 */
typedef struct shared_data {
  uint64_t position;
  pthread_mutex_t can_access_position;
  uint64_t thread_count;
  uint64_t hits_available;
} shared_data_t;

/**
 * @brief Estructura que almacena los datos privados de cada hilo.
 * 
 * Contiene un identificador único para el hilo (número de hilo), el número de hits
 * que el hilo realizará, y un puntero a los datos compartidos.
 */
typedef struct private_data {
  uint64_t thread_number;
  shared_data_t* shared_data;
  uint64_t hits;
} private_data_t;

/**
 * @brief Función que ejecuta cada hilo para reducir el número de hits disponibles.
 * 
 * @param data Puntero a los datos privados del hilo.
 * @return void* Retorna NULL al finalizar.
 * 
 * Esta función se asegura de que cada hilo realice una cantidad de hits y luego
 * reduzca el número de hits disponibles de manera segura usando un mutex.
 */
void* hit(void* data);

/**
 * @brief Crea y ejecuta los hilos que compiten por los hits disponibles.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos entre los hilos.
 * @return int Retorna EXIT_SUCCESS si todo sale bien o un código de error en caso contrario.
 * 
 * Esta función crea los hilos necesarios para competir por los hits disponibles,
 * inicia la ejecución de cada hilo y luego espera a que todos terminen antes de limpiar los recursos.
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Función principal del programa.
 * 
 * @param argc Cantidad de argumentos de línea de comandos.
 * @param argv Array de argumentos de línea de comandos.
 * @return int Retorna EXIT_SUCCESS si todo sale bien o un código de error en caso contrario.
 * 
 * La función principal inicializa la semilla aleatoria, establece el número de hilos 
 * y la cantidad de golpes disponibles, luego crea los hilos que competirán por los hits disponibles.
 */
int main(int argc, char* argv[]) {
  srand(time(NULL));  // Inicializa la semilla aleatoria

  int error = EXIT_SUCCESS;
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  uint64_t hits_available = 10;  // Valor por defecto para los hits disponibles

  if (argc >= 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) != 1) {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }

  if (argc >= 3) {
    if (sscanf(argv[2], "%" SCNu64, &hits_available) != 1) {
      fprintf(stderr, "Error: invalid hits count\n");
      return 12;
    }
  }

  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    shared_data->position = 0;
    shared_data->hits_available = hits_available;
    error = pthread_mutex_init(&shared_data->can_access_position, /*attr*/ NULL);
    if (error == EXIT_SUCCESS) {
      shared_data->thread_count = thread_count;

      struct timespec start_time, finish_time;
      clock_gettime(CLOCK_MONOTONIC, &start_time);

      error = create_threads(shared_data);

      clock_gettime(CLOCK_MONOTONIC, &finish_time);
      double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
        (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;

      printf("Execution time: %.9lfs\n", elapsed_time);

      pthread_mutex_destroy(&shared_data->can_access_position);
      free(shared_data);
    } else {
      fprintf(stderr, "Error: could not init mutex\n");
      return 13;
    }
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    return 12;
  }
  return error;
}

/**
 * @brief Crea los hilos para realizar los hits disponibles.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos.
 * @return int Retorna EXIT_SUCCESS si todo salió bien, o un código de error si falló.
 * 
 * Esta función inicializa la cantidad de hits disponibles y crea los hilos, 
 * asignando a cada hilo un número aleatorio de hits. Luego, espera a que todos los hilos terminen.
 */
int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  pthread_t* threads = (pthread_t*)malloc(shared_data->thread_count *
  sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*)calloc
  (shared_data->thread_count, sizeof(private_data_t));

  if (threads && private_data) {
    printf("Time to get candy! Hits available: %" PRIu64 "\n",
    shared_data->hits_available);
    for (uint64_t thread_number = 0; thread_number <
    shared_data->thread_count; ++thread_number) {
      unsigned int seed = time(NULL) + thread_number;
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].shared_data = shared_data;
      private_data[thread_number].hits = rand_r(&seed) % 10;

      // Ajustar hits si son mayores que los hits disponibles
      if (private_data[thread_number].hits > shared_data->hits_available) {
        private_data[thread_number].hits = shared_data->hits_available;
      }

      error = pthread_create(&threads[thread_number], /*attr*/ NULL,
        hit, /*arg*/ &private_data[thread_number]);
      if (error != EXIT_SUCCESS) {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count;
    ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n",
    shared_data->thread_count);
    error = 22;
  }

  return error;
}

/**
 * @brief Función que ejecuta cada hilo para reducir los hits disponibles.
 * 
 * @param data Puntero a la estructura privada del hilo.
 * @return void* Retorna NULL al finalizar.
 * 
 * Esta función se asegura de que cada hilo intente realizar una cantidad de hits, 
 * disminuyendo de manera segura los hits disponibles usando un mutex para evitar 
 * condiciones de carrera.
 */
void* hit(void* data) {
  assert(data);
  private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = private_data->shared_data;

  pthread_mutex_lock(&shared_data->can_access_position);

  if (shared_data->hits_available == 0) {
    printf("Thread %" PRIu64 " my hits: %" PRIu64
     ": No more hits available, I couldn't hit...\n",
           private_data->thread_number, private_data->hits);
    pthread_mutex_unlock(&shared_data->can_access_position);
    return NULL;
  }

  // Ajustar hits si son mayores que los hits disponibles
  if (private_data->hits > shared_data->hits_available) {
    private_data->hits = shared_data->hits_available;
  }

  // Reducir hits disponibles
  shared_data->hits_available -= private_data->hits;
  ++shared_data->position;

  if (shared_data->hits_available == 0) {
    printf("Thread %" PRIu64 " hits: %" PRIu64 ": I destroyed the pinata!\n",
           private_data->thread_number, private_data->hits);
  } else {
    printf("Thread %" PRIu64 " hits: %" PRIu64 ": Pinata still has hits left!\n",
           private_data->thread_number, private_data->hits);
  }

  pthread_mutex_unlock(&shared_data->can_access_position);
  return NULL;
}
