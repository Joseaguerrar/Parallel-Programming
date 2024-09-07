// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// Simulates a producer and a consumer that share a bounded buffer

// @see `man feature_test_macros`
#define _DEFAULT_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <unistd.h>

/**
 * @brief Códigos de error utilizados por el programa.
 */
enum {
  ERR_NOMEM_SHARED = EXIT_FAILURE + 1, 
  // Error al no poder asignar memoria compartida.
  ERR_NOMEM_BUFFER,                    
  // Error al no poder asignar memoria para el búfer.
  ERR_NO_ARGS,                         
  // Error cuando no se proporcionan argumentos.
  ERR_BUFFER_CAPACITY,                 
  // Error cuando la capacidad del búfer es inválida.
  ERR_ROUND_COUNT,                     
  // Error cuando el número de rondas es inválido.
  ERR_MIN_PROD_DELAY,                  
  // Error cuando el retardo mínimo del productor es inválido.
  ERR_MAX_PROD_DELAY,                  
  // Error cuando el retardo máximo del productor es inválido.
  ERR_MIN_CONS_DELAY,                  
  // Error cuando el retardo mínimo del consumidor es inválido.
  ERR_MAX_CONS_DELAY,                  
  // Error cuando el retardo máximo del consumidor es inválido.
  ERR_CREATE_THREAD                    
  // Error al no poder crear un hilo.
};

/**
 * @brief Estructura que almacena los datos compartidos entre los hilos.
 */
typedef struct {
  size_t thread_count;
  // Número de hilos a crear.
  size_t buffer_capacity;
  // Capacidad del búfer compartido.
  double* buffer;
  // Puntero al búfer compartido.
  size_t rounds;
  // Número de rondas de producción/consumo.
  useconds_t producer_min_delay;
  // Retardo mínimo para los hilos productores.
  useconds_t producer_max_delay;
  // Retardo máximo para los hilos productores.
  useconds_t consumer_min_delay;
  // Retardo mínimo para los hilos consumidores.
  useconds_t consumer_max_delay;
  // Retardo máximo para los hilos consumidores.
} shared_data_t;

/**
 * @brief Estructura que almacena los datos privados de cada hilo.
 */
typedef struct {
  size_t thread_number;
  // Número del hilo.
  shared_data_t* shared_data;
  // Puntero a la estructura de datos compartidos.
} private_data_t;

/**
 * @brief Analiza los argumentos de línea de comandos para inicializar los datos compartidos.
 * 
 * @param argc Número de argumentos en la línea de comandos.
 * @param argv Array de argumentos de la línea de comandos.
 * @param shared_data Puntero a la estructura de datos compartidos.
 * @return int Devuelve EXIT_SUCCESS si los argumentos son válidos, en caso contrario devuelve un código de error.
 */
int analyze_arguments(int argc, char* argv[], shared_data_t* shared_data);

/**
 * @brief Crea los hilos productores y consumidores.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos.
 * @return int Devuelve EXIT_SUCCESS si los hilos se crean correctamente, en caso contrario devuelve un código de error.
 */
int create_threads(shared_data_t* shared_data);

/**
 * @brief Función que ejecuta el hilo productor.
 * 
 * @param data Puntero a los datos privados del hilo.
 * @return void* Siempre devuelve NULL.
 */
void* produce(void* data);

/**
 * @brief Función que ejecuta el hilo consumidor.
 * 
 * @param data Puntero a los datos privados del hilo.
 * @return void* Siempre devuelve NULL.
 */
void* consume(void* data);

/**
 * @brief Genera un valor aleatorio entre dos límites dados.
 * 
 * @param min Valor mínimo.
 * @param max Valor máximo.
 * @return useconds_t Valor aleatorio entre min y max.
 */
useconds_t random_between(useconds_t min, useconds_t max);

/**
 * @brief Función principal que coordina la simulación productor-consumidor.
 * 
 * Esta función se encarga de asignar la memoria compartida, analizar los argumentos 
 * de la línea de comandos, crear los hilos y medir el tiempo de ejecución.
 * 
 * @param argc Número de argumentos en la línea de comandos.
 * @param argv Array de argumentos de la línea de comandos.
 * @return int Devuelve EXIT_SUCCESS si la ejecución es exitosa, en caso contrario devuelve un código de error.
 */
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  // Asignar estructura de datos compartidos
  shared_data_t* shared_data = (shared_data_t*)
    calloc(1, sizeof(shared_data_t));

  if (shared_data) {
    // Analizar los argumentos de la línea de comandos
    error = analyze_arguments(argc, argv, shared_data);
    if (error == EXIT_SUCCESS) {
      // Asignar el búfer para los datos compartidos
      shared_data->buffer = (double*)
        calloc(shared_data->buffer_capacity, sizeof(double));
      if (shared_data->buffer) {
        // Sembrar el generador de números aleatorios
        unsigned int seed = 0u;
        getrandom(&seed, sizeof(seed), GRND_NONBLOCK);
        srandom(seed);

        // Registrar el tiempo de inicio de la ejecución
        struct timespec start_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // Crear los hilos productores y consumidores
        error = create_threads(shared_data);

        // Registrar el tiempo de finalización de la ejecución
        struct timespec finish_time;
        clock_gettime(CLOCK_MONOTONIC, &finish_time);

        // Calcular y mostrar el tiempo de ejecución
        double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
          (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
        printf("Tiempo de ejecución: %.9lfs\n", elapsed);

        // Liberar el búfer asignado
        free(shared_data->buffer);
      } else {
        fprintf(stderr, "error: no se pudo crear el búfer\n");
        error = ERR_NOMEM_BUFFER;
      }
    }

    // Liberar la estructura de datos compartidos
    free(shared_data);
  } else {
    fprintf(stderr, "Error: no se pudieron asignar los datos compartidos\n");
    error = ERR_NOMEM_SHARED;
  }

  return error;
}

/**
 * @brief Analiza los argumentos de la línea de comandos para inicializar los datos compartidos.
 * 
 * Esta función verifica y asigna los valores proporcionados por el usuario a la estructura
 * de datos compartidos. Comprueba que todos los argumentos son válidos y, si no lo son,
 * informa del error correspondiente.
 * 
 * @param argc Número de argumentos proporcionados.
 * @param argv Array de cadenas de los argumentos.
 * @param shared_data Puntero a la estructura de datos compartidos a inicializar.
 * @return int Devuelve EXIT_SUCCESS si los argumentos son válidos, de lo contrario devuelve un código de error.
 */
int analyze_arguments(int argc, char* argv[], shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  if (argc == 7) {
    if (sscanf(argv[1], "%zu", &shared_data->buffer_capacity) != 1
      || shared_data->buffer_capacity == 0) {
        fprintf(stderr, "error: capacidad de búfer inválida\n");
        error = ERR_BUFFER_CAPACITY;
    } else if (sscanf(argv[2], "%zu", &shared_data->rounds) != 1
      || shared_data->rounds == 0) {
        fprintf(stderr, "error: número de rondas inválido\n");
        error = ERR_ROUND_COUNT;
    } else if (sscanf(argv[3], "%u", &shared_data->producer_min_delay) != 1) {
        fprintf(stderr, "error: retardo mínimo del productor inválido\n");
        error = ERR_MIN_PROD_DELAY;
    } else if (sscanf(argv[4], "%u", &shared_data->producer_max_delay) != 1) {
        fprintf(stderr, "error: retardo máximo del productor inválido\n");
        error = ERR_MAX_PROD_DELAY;
    } else if (sscanf(argv[5], "%u", &shared_data->consumer_min_delay) != 1) {
        fprintf(stderr, "error: retardo mínimo del consumidor inválido\n");
        error = ERR_MIN_CONS_DELAY;
    } else if (sscanf(argv[6], "%u", &shared_data->consumer_max_delay) != 1) {
        fprintf(stderr, "error: retardo máximo del consumidor inválido\n");
        error = ERR_MAX_CONS_DELAY;
    }
  } else {
    fprintf(stderr, "Uso: prod_cons_bound buffer_capacity rounds"
      " producer_min_delay producer_max_delay"
      " consumer_min_delay consumer_max_delay\n");
      error = ERR_NO_ARGS;
  }
  return error;
}

/**
 * @brief Crea los hilos productor y consumidor.
 * 
 * Esta función se encarga de crear los hilos para el productor y el consumidor,
 * manejando posibles errores en la creación de los hilos y esperando a que ambos
 * terminen su ejecución con `pthread_join`.
 * 
 * @param shared_data Puntero a la estructura de datos compartidos.
 * @return int Devuelve EXIT_SUCCESS si los hilos se crean y sincronizan correctamente, de lo contrario devuelve un código de error.
 */
int create_threads(shared_data_t* shared_data) {
  assert(shared_data);
  int error = EXIT_SUCCESS;

  pthread_t producer, consumer;
  error = pthread_create(&producer, /*attr*/ NULL, produce, shared_data);
  if (error == EXIT_SUCCESS) {
    error = pthread_create(&consumer, /*attr*/ NULL, consume, shared_data);
    if (error != EXIT_SUCCESS) {
      fprintf(stderr, "error: no se pudo crear el hilo consumidor\n");
      error = ERR_CREATE_THREAD;
    }
  } else {
    fprintf(stderr, "error: no se pudo crear el hilo productor\n");
    error = ERR_CREATE_THREAD;
  }

  if (error == EXIT_SUCCESS) {
    pthread_join(producer, /*value_ptr*/ NULL);
    pthread_join(consumer, /*value_ptr*/ NULL);
  }

  return error;
}

void* produce(void* data) {
  // const private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = (shared_data_t*)data;
  size_t count = 0;
  for (size_t round = 0; round < shared_data->rounds; ++round) {
    for (size_t index = 0; index < shared_data->buffer_capacity; ++index) {
      usleep(1000 * random_between(shared_data->producer_min_delay
        , shared_data->producer_max_delay));
      shared_data->buffer[index] = ++count;
      printf("Produced %lg\n", shared_data->buffer[index]);
    }
  }

  return NULL;
}

void* consume(void* data) {
  // const private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = (shared_data_t*)data;
  for (size_t round = 0; round < shared_data->rounds; ++round) {
    for (size_t index = 0; index < shared_data->buffer_capacity; ++index) {
      double value = shared_data->buffer[index];
      usleep(1000 * random_between(shared_data->consumer_min_delay
        , shared_data->consumer_max_delay));
      printf("\tConsumed %lg\n", value);
    }
  }

  return NULL;
}

useconds_t random_between(useconds_t min, useconds_t max) {
  return min + (max > min ? (random() % (max - min)) : 0);
}