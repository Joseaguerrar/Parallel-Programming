// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/random.h>
#include <stdio.h>

#include "common.h"
#include "consumer.h"
#include "producer.h"
#include "simulation.h"

/**
 * @brief Analiza los argumentos proporcionados en la línea de comandos.
 * 
 * Esta función valida y asigna los argumentos necesarios para la simulación, como el número
 * de productores, consumidores, y las unidades a procesar. 
 * 
 * @param simulation Puntero a la estructura de simulación.
 * @param argc Número de argumentos en la línea de comandos.
 * @param argv Array de cadenas que contiene los argumentos de la línea de comandos.
 * @return int Devuelve 0 si los argumentos son válidos, de lo contrario devuelve un código de error.
 */
int analyze_arguments(simulation_t* simulation, int argc, char* argv[]);

/**
 * @brief Crea hilos de productores y consumidores.
 * 
 * Esta función se encarga de crear los hilos correspondientes a los productores
 * y consumidores en función de la configuración de la simulación.
 * 
 * @param simulation Puntero a la estructura de simulación.
 * @return int Devuelve 0 si los hilos se crean correctamente, de lo contrario devuelve un código de error.
 */
int create_consumers_producers(simulation_t* simulation);

/**
 * @brief Une los hilos creados.
 * 
 * Esta función espera a que todos los hilos de productores y consumidores terminen su ejecución.
 * 
 * @param count Número de hilos a unir.
 * @param threads Array de hilos.
 * @return int Devuelve 0 si todos los hilos se unen correctamente, de lo contrario devuelve un código de error.
 */
int join_threads(size_t count, pthread_t* threads);

/**
 * @brief Crea e inicializa una nueva simulación.
 * 
 * Esta función asigna memoria y recursos para ejecutar una simulación. Inicializa las estructuras
 * y los mutexes y semáforos necesarios para la sincronización entre hilos.
 * 
 * @return simulation_t* Devuelve un puntero a la estructura de simulación creada, o NULL si falla.
 */
simulation_t* simulation_create() {
  simulation_t* simulation = (simulation_t*) calloc(1, sizeof(simulation_t));
  if (simulation) {
    simulation->unit_count = 0;
    // Inicializa el número de unidades a procesar.

    simulation->producer_count = 0;
    // Inicializa el número de productores.

    simulation->consumer_count = 0;
    // Inicializa el número de consumidores.

    simulation->producer_min_delay = 0;
    // Inicializa el retardo mínimo para los productores.

    simulation->producer_max_delay = 0;
    // Inicializa el retardo máximo para los productores.

    simulation->consumer_min_delay = 0;
    // Inicializa el retardo mínimo para los consumidores.

    simulation->consumer_max_delay = 0;
    // Inicializa el retardo máximo para los consumidores.

    queue_init(&simulation->queue);
    // Inicializa la cola compartida entre productores y consumidores.

    pthread_mutex_init(&simulation->can_access_next_unit, /* attr */ NULL);
    // Inicializa el mutex para acceder a la siguiente unidad a producir.

    simulation->next_unit = 0;
    // Inicializa el índice de la próxima unidad a producir.

    sem_init(&simulation->can_consume, /* pshared */ 0, /* value */ 0);
    /* Inicializa el semáforo para controlar
    cuándo los consumidores pueden consumir.*/

    pthread_mutex_init(&simulation->can_access_consumed_count, /* attr */ NULL);
    /* Inicializa el mutex para controlar
    el acceso al conteo de unidades consumidas.*/

    simulation->consumed_count = 0;
    // Inicializa el contador de unidades consumidas.
  }
  return simulation;
}

/**
 * @brief Destruye la simulación y libera los recursos.
 * 
 * Esta función libera la memoria y destruye los mutexes y semáforos utilizados durante la simulación.
 * 
 * @param simulation Puntero a la estructura de simulación que se va a destruir.
 */
void simulation_destroy(simulation_t* simulation) {
  assert(simulation);
  pthread_mutex_destroy(&simulation->can_access_consumed_count);
  // Destruye el mutex para el conteo de unidades consumidas.

  sem_destroy(&simulation->can_consume);
  // Destruye el semáforo que controla el consumo.

  pthread_mutex_destroy(&simulation->can_access_next_unit);
  // Destruye el mutex para acceder a la siguiente unidad.

  queue_destroy(&simulation->queue);
  // Destruye la cola compartida.

  free(simulation);
  // Libera la memoria asignada para la simulación.
}

/**
 * @brief Ejecuta la simulación.
 * 
 * Esta función controla el ciclo de ejecución de la simulación, midiendo el tiempo
 * de ejecución y coordinando la creación de hilos para productores y consumidores.
 * 
 * @param simulation Puntero a la estructura de simulación.
 * @param argc Número de argumentos proporcionados en la línea de comandos.
 * @param argv Array de cadenas con los argumentos de la línea de comandos.
 * @return int Devuelve 0 si la simulación se ejecuta correctamente, de lo contrario devuelve un código de error.
 */
int simulation_run(simulation_t* simulation, int argc, char* argv[]) {
  int error = analyze_arguments(simulation, argc, argv);
  // Analiza y valida los argumentos proporcionados.

  if (error == EXIT_SUCCESS) {
    unsigned int seed = 0;
    getrandom(&seed, sizeof(seed), GRND_NONBLOCK);
    // Genera una semilla aleatoria para los retardos.

    srandom(seed);

    struct timespec start_time;
    clock_gettime(/*clk_id*/CLOCK_MONOTONIC, &start_time);
    // Registra el tiempo de inicio de la simulación.

    error = create_consumers_producers(simulation);
    // Crea los hilos de productores y consumidores.

    struct timespec finish_time;
    clock_gettime(/*clk_id*/CLOCK_MONOTONIC, &finish_time);
    // Registra el tiempo de finalización de la simulación.

    double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
      (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
    // Calcula el tiempo total de ejecución de la simulación.

    printf("execution time: %.9lfs\n", elapsed);
    // Imprime el tiempo de ejecución.
  }
  return error;
}
/**
 * @brief Analiza los argumentos proporcionados en la línea de comandos.
 * 
 * Esta función valida los argumentos proporcionados por el usuario, como el número de unidades,
 * productores, consumidores y retardos. Si los argumentos no son válidos, se imprime un mensaje
 * de error y se devuelve un código de error.
 * 
 * @param simulation Puntero a la estructura de simulación.
 * @param argc Número de argumentos en la línea de comandos.
 * @param argv Array de cadenas que contiene los argumentos de la línea de comandos.
 * @return int Devuelve 0 si los argumentos son válidos, o un código de error si fallan.
 */
int analyze_arguments(simulation_t* simulation, int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  if (argc == 8) {
    if (sscanf(argv[1], "%zu", &simulation->unit_count) != 1
      || simulation->unit_count == 0) {
        fprintf(stderr, "error: invalid unit count\n");
        error = ERR_UNIT_COUNT;
        // Error si el número de unidades es inválido o igual a cero.
    } else if (sscanf(argv[2], "%zu", &simulation->producer_count) != 1
      || simulation->producer_count == 0) {
        fprintf(stderr, "error: invalid producer count\n");
        error = ERR_PRODUCER_COUNT;
        // Error si el número de productores es inválido o igual a cero.
    } else if (sscanf(argv[3], "%zu", &simulation->consumer_count) != 1
      || simulation->consumer_count == 0) {
        fprintf(stderr, "error: invalid consumer count\n");
        error = ERR_CONSUMER_COUNT;
        // Error si el número de consumidores es inválido o igual a cero.
    } else if (sscanf(argv[4], "%u", &simulation->producer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min producer delay\n");
        error = ERR_MIN_PROD_DELAY;
        // Error si el retraso mínimo del productor es inválido.
    } else if (sscanf(argv[5], "%u", &simulation->producer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max producer delay\n");
        error = ERR_MAX_PROD_DELAY;
        // Error si el retraso máximo del productor es inválido.
    } else if (sscanf(argv[6], "%u", &simulation->consumer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min consumer delay\n");
        error = ERR_MIN_CONS_DELAY;
        // Error si el retraso mínimo del consumidor es inválido.
    } else if (sscanf(argv[7], "%u", &simulation->consumer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max consumer delay\n");
        error = ERR_MAX_CONS_DELAY;
        // Error si el retraso máximo del consumidor es inválido.
    }
  } else {
    fprintf(stderr, "usage: producer_consumer buffer_capacity rounds"
      " producer_min_delay producer_max_delay"
      " consumer_min_delay consumer_max_delay\n");
    // Muestra el formato correcto de los argumentos esperados.
    error = ERR_NO_ARGS;
  }
  return error;
}

/**
 * @brief Crea un grupo de hilos.
 * 
 * Esta función crea un grupo de hilos que ejecutan una subrutina específica. Si ocurre
 * un error al crear uno de los hilos, la función unirá los hilos ya creados y devolverá NULL.
 * 
 * @param count Número de hilos a crear.
 * @param subroutine Función que cada hilo ejecutará.
 * @param data Puntero a los datos que se pasarán a cada hilo.
 * @return pthread_t* Devuelve un puntero al array de hilos creados, o NULL si ocurre un error.
 */
pthread_t* create_threads(size_t count, void*(*subroutine)(void*), void* data) {
  pthread_t* threads = (pthread_t*) calloc(count, sizeof(pthread_t));
  // Reserva memoria para los hilos.

  if (threads) {
    for (size_t index = 0; index < count; ++index) {
      if (pthread_create(&threads[index], /*attr*/ NULL, subroutine, data)
         == EXIT_SUCCESS) {
        // Crea cada hilo con la subrutina especificada.
      } else {
        fprintf(stderr, "error: could not create thread %zu\n", index);
        join_threads(index, threads);
        // Si no se puede crear un hilo, se unen los hilos creados previamente.
        return NULL;
      }
    }
  }
  return threads;
}

/**
 * @brief Une los hilos creados.
 * 
 * Esta función espera a que todos los hilos del array proporcionado terminen su ejecución.
 * 
 * @param count Número de hilos a unir.
 * @param threads Array de hilos.
 * @return int Devuelve 0 si todos los hilos se unen correctamente, o un código de error si falla.
 */
int join_threads(size_t count, pthread_t* threads) {
  int error = EXIT_SUCCESS;
  for (size_t index = 0; index < count; ++index) {
    // todo: sum could not be right
    error += pthread_join(threads[index], /*value_ptr*/ NULL);
    // Une el hilo en el índice actual.
  }
  free(threads);
  // Libera la memoria asignada para los hilos.
  return error;
}

/**
 * @brief Crea los hilos de productores y consumidores.
 * 
 * Esta función crea los hilos de productores y consumidores para la simulación. Si los hilos
 * no pueden ser creados, devuelve un error.
 * 
 * @param simulation Puntero a la estructura de simulación.
 * @return int Devuelve 0 si los hilos se crean y se unen correctamente, o un código de error si falla.
 */
int create_consumers_producers(simulation_t* simulation) {
  assert(simulation);
  int error = EXIT_SUCCESS;

  pthread_t* producers = create_threads(simulation->producer_count, produce
    , simulation);
  // Crea los hilos de los productores.

  pthread_t* consumers = create_threads(simulation->consumer_count, consume
    , simulation);
  // Crea los hilos de los consumidores.

  if (producers && consumers) {
    join_threads(simulation->producer_count, producers);
    // Une los hilos de los productores.

    join_threads(simulation->consumer_count, consumers);
    // Une los hilos de los consumidores.
  } else {
    fprintf(stderr, "error: could not create threads\n");
    error = ERR_CREATE_THREAD;
    // Error si no se pudieron crear los hilos.
  }

  return error;
}
