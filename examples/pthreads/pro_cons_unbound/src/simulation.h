// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#ifndef SIMULATION_H
#define SIMULATION_H

/**
 * @file simulation.h
 * @brief Declaración de las funciones de la simulación.
 * 
 * Este archivo de cabecera contiene las funciones relacionadas con la creación, 
 * ejecución y destrucción de una simulación. La estructura `simulation_t` es opaca 
 * para ocultar los detalles internos de la implementación.
 */

/**
 * @brief Declaración de un tipo opaco de simulación.
 * 
 * La estructura `simulation_t` se define como opaca para que los detalles de su
 * implementación estén ocultos fuera del archivo fuente correspondiente.
 */
typedef struct simulation simulation_t;  
// Tipo de dato opaco para representar una simulación.

/**
 * @brief Crea e inicializa una nueva simulación.
 * 
 * Esta función asigna memoria y configura los recursos necesarios para ejecutar 
 * una simulación. Devuelve un puntero a la nueva estructura de simulación.
 * 
 * @return simulation_t* Devuelve un puntero a la estructura de simulación creada.
 * Si no se pudo crear, devuelve NULL.
 */
simulation_t* simulation_create();

/**
 * @brief Ejecuta la simulación con los argumentos proporcionados.
 * 
 * Esta función ejecuta el ciclo principal de la simulación utilizando los parámetros 
 * proporcionados en `argc` y `argv`. Controla la lógica de la simulación basada en 
 * estos argumentos.
 * 
 * @param simulation Puntero a la estructura de la simulación que se va a ejecutar.
 * @param argc Número de argumentos proporcionados en la línea de comandos.
 * @param argv Array de cadenas de los argumentos de la línea de comandos.
 * @return int Devuelve 0 si la simulación se ejecuta correctamente, o un código de error si falla.
 */
int simulation_run(simulation_t* simulation, int argc, char* argv[]);

/**
 * @brief Destruye la simulación y libera los recursos.
 * 
 * Esta función limpia y libera todos los recursos utilizados por la simulación,
 * incluyendo la memoria asignada para la estructura `simulation_t`.
 * 
 * @param simulation Puntero a la estructura de la simulación que se va a destruir.
 */
void simulation_destroy(simulation_t* simulation);
#endif  // SIMULATION_H