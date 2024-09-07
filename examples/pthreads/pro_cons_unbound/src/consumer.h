// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#ifndef CONSUMER_H
#define CONSUMER_H

/**
 * @file consumer.h
 * @brief Declaración de la función `consume` para el hilo consumidor.
 * 
 * Este archivo de cabecera contiene la declaración de la función `consume`,
 * que implementa la lógica del hilo consumidor en la simulación.
 */

/**
 * @brief Función que ejecuta el hilo consumidor.
 * 
 * Esta función simula el proceso de consumo en la simulación. Los consumidores
 * toman elementos de una cola compartida, los procesan y esperan un tiempo
 * aleatorio entre un retardo mínimo y máximo.
 * 
 * @param data Puntero a los datos de la simulación (generalmente una estructura de simulación).
 * @return void* Devuelve NULL al finalizar el consumo.
 */
void* consume(void* data);

#endif  // CONSUMER_H
