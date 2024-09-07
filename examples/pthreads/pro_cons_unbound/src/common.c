// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <stdlib.h>

#include "common.h"

/**
 * @brief Genera un valor aleatorio entre dos límites.
 * 
 * Esta función devuelve un valor aleatorio entre los límites mínimo (min) y 
 * máximo (max) especificados. Si el valor máximo es mayor que el mínimo, 
 * genera un número en ese rango; de lo contrario, devuelve el valor mínimo.
 * 
 * @param min Valor mínimo del rango.
 * @param max Valor máximo del rango.
 * @return useconds_t Un valor aleatorio en microsegundos entre min y max.
 */
useconds_t random_between(useconds_t min, useconds_t max) {
  return min + (max > min ? (random() % (max - min)) : 0);
}