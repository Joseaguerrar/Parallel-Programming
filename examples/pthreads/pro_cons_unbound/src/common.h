// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#ifndef COMMON_H
#define COMMON_H

#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

/**
 * @brief Códigos de error utilizados en la simulación.
 * 
 * Estos códigos indican diferentes tipos de errores que pueden ocurrir durante
 * la ejecución del programa, como errores de memoria o errores al crear hilos.
 */
enum {
  ERR_NOMEM_SHARED = EXIT_FAILURE + 1, 
  // Error al no poder asignar memoria compartida.

  ERR_NOMEM_BUFFER,                    
  // Error al no poder asignar memoria para el búfer.

  ERR_NO_ARGS,                         
  // Error cuando no se proporcionan argumentos.

  ERR_UNIT_COUNT,                      
  // Error cuando el número de unidades es inválido.

  ERR_PRODUCER_COUNT,                  
  // Error cuando el número de productores es inválido.

  ERR_CONSUMER_COUNT,                  
  // Error cuando el número de consumidores es inválido.

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
 * @brief Estructura que almacena los datos de la simulación.
 * 
 * Esta estructura contiene los parámetros necesarios para ejecutar la simulación, 
 * como el número de productores y consumidores, los retrasos y la cola compartida 
 * para coordinar las operaciones entre productores y consumidores.
 */
typedef struct simulation {
  size_t unit_count;                
  // Número total de unidades a procesar.

  size_t producer_count;            
  // Número de hilos productores.

  size_t consumer_count;            
  // Número de hilos consumidores.

  useconds_t producer_min_delay;    
  // Retardo mínimo para los productores.

  useconds_t producer_max_delay;    
  // Retardo máximo para los productores.

  useconds_t consumer_min_delay;    
  // Retardo mínimo para los consumidores.

  useconds_t consumer_max_delay;    
  // Retardo máximo para los consumidores.

  queue_t queue;                    
  // Cola compartida entre productores y consumidores.

  pthread_mutex_t can_access_next_unit; 
  // Mutex para acceder a la siguiente unidad.

  size_t next_unit;                 
  // Índice de la siguiente unidad a ser procesada.

  sem_t can_consume;                
  // Semáforo para controlar cuándo se puede consumir.

  pthread_mutex_t can_access_consumed_count; 
  // Mutex para acceder al conteo de unidades consumidas.

  size_t consumed_count;            
  // Número de unidades consumidas.
} simulation_t;

/**
 * @brief Genera un valor aleatorio entre dos límites.
 * 
 * Esta función devuelve un valor aleatorio en microsegundos entre los límites 
 * mínimo (min) y máximo (max) especificados. Si el valor máximo es mayor que 
 * el mínimo, genera un número aleatorio en ese rango; de lo contrario, devuelve 
 * el valor mínimo.
 * 
 * @param min Retardo mínimo en microsegundos.
 * @param max Retardo máximo en microsegundos.
 * @return useconds_t Valor aleatorio en microsegundos entre min y max.
 */
useconds_t random_between(useconds_t min, useconds_t max);

#endif  // COMMON_H
