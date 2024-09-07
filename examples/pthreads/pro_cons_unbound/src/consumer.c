// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <stdbool.h>
#include <stdio.h>

#include "common.h"
#include "consumer.h"

/**
 * @brief Función que ejecuta el hilo consumidor.
 * 
 * Esta función simula el proceso de consumo. Los consumidores intentan 
 * consumir unidades de la cola compartida. Cada consumidor verifica si hay 
 * más productos para consumir y, si es así, reserva uno para sí mismo, 
 * lo consume y espera un tiempo aleatorio entre los retardos mínimo y máximo.
 * 
 * @param data Puntero a la estructura de datos de la simulación (simulation_t).
 * @return void* Siempre devuelve NULL.
 */
void* consume(void* data) {
  simulation_t* simulation = (simulation_t*)data;

  while (true) {
    // lock(can_access_consumed_count)
    pthread_mutex_lock(&simulation->can_access_consumed_count);
    // Bloquea el acceso al contador de unidades consumidas.

    if (simulation->consumed_count < simulation->unit_count) {
      // Reserve the next product to me
      ++simulation->consumed_count;
      // Incrementa el contador de unidades consumidas por el hilo actual.
    } else {
      // unlock(can_access_consumed_count)
      pthread_mutex_unlock(&simulation->can_access_consumed_count);
      // Desbloquea el acceso al contador de unidades consumidas.

      // break while
      break;
      // Sale del bucle si ya se han consumido todas las unidades.
    }

    // unlock(can_access_consumed_count)
    pthread_mutex_unlock(&simulation->can_access_consumed_count);
    // Desbloquea el acceso al contador de unidades consumidas.

    // wait(can_consume)
    sem_wait(&simulation->can_consume);
    // Espera a que haya productos disponibles para consumir.

    size_t value = 0;
    queue_dequeue(&simulation->queue, &value);
    // Extrae un producto de la cola compartida.

    printf("\tConsuming %zu\n", value);
    // Imprime el valor consumido.

    usleep(1000 * random_between(simulation->consumer_min_delay,
      simulation->consumer_max_delay));
    /* Espera un tiempo aleatorio entre el retardo
    mínimo y máximo del consumidor.*/
  }

  return NULL;
}
