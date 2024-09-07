// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <stdio.h>

#include "common.h"
#include "producer.h"

/**
 * @brief Función que ejecuta el hilo productor.
 * 
 * Esta función simula el proceso de producción en la simulación. Los productores
 * generan unidades de trabajo y las colocan en una cola compartida. Si hay unidades
 * pendientes, el productor toma una unidad y la procesa. Si no, el hilo termina.
 * 
 * @param data Puntero a los datos de la simulación (simulation_t).
 * @return void* Devuelve NULL al finalizar.
 */
void* produce(void* data) {
  simulation_t* simulation = (simulation_t*)data;

  while (true) {
    // declare my_unit := 0
    size_t my_unit = 0;
    // Declara una unidad local para ser producida.

    // lock(can_access_next_unit)
    pthread_mutex_lock(&simulation->can_access_next_unit);
    // Bloquea el acceso a la siguiente unidad para evitar condiciones de carrera.

    // If is there pending work, take a unit for producing
    if (simulation->next_unit < simulation->unit_count) {
      my_unit = ++simulation->next_unit;
      // Si hay unidades pendientes, asigna una al productor.
    } else {
      // unlock(can_access_next_unit)
      pthread_mutex_unlock(&simulation->can_access_next_unit);
      // Desbloquea el acceso a la siguiente unidad si no hay trabajo pendiente.

      // break while
      break;
      // Termina el bucle si no quedan más unidades por producir.
    }
    // unlock(can_access_next_unit)
    pthread_mutex_unlock(&simulation->can_access_next_unit);
    // Desbloquea el acceso a la siguiente unidad después de asignarla.

    usleep(1000 * random_between(simulation->producer_min_delay,
      simulation->producer_max_delay));
    // Simula el retardo entre la producción de las unidades.

    queue_enqueue(&simulation->queue, my_unit);
    // Encola la unidad producida en la cola compartida.

    printf("Produced %zu\n", my_unit);
    // Imprime el número de la unidad producida.

    // signal(can_consume)
    sem_post(&simulation->can_consume);
    // Señala que hay una nueva unidad disponible para ser consumida.
  }

  return NULL;
}
