// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#ifndef PRODUCER_H
#define PRODUCER_H

/**
 * @file producer.h
 * @brief Declaración de la función `produce` para el hilo productor.
 * 
 * Este archivo de cabecera contiene la declaración de la función `produce`,
 * que implementa la lógica del hilo productor en la simulación.
 */

/**
 * @brief Función que ejecuta el hilo productor.
 * 
 * Esta función se encarga de generar unidades de trabajo en la simulación. 
 * Los productores toman unidades pendientes, las procesan y las colocan en 
 * una cola compartida. La función maneja la sincronización mediante mutexes 
 * y semáforos para asegurar el acceso correcto a los recursos compartidos.
 * 
 * @param data Puntero a los datos de la simulación (generalmente una estructura de simulación).
 * @return void* Devuelve NULL cuando el hilo productor termina su ejecución.
 */
void* produce(void* data);

#endif  // PRODUCER_H
