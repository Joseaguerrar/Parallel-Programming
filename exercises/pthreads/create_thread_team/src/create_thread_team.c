// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @file hello_iw_pri.c
 * @brief Programa de ejemplo que utiliza hilos y estructuras enlazadas.
 *
 * Este archivo contiene un programa en C que crea múltiples hilos y los 
 * organiza en una lista enlazada simple. Cada hilo ejecuta una función que 
 * imprime su número de hilo y el número total de hilos creados.
 */

/**
 * @brief Estructura que contiene datos privados para cada hilo.
 *
 * Esta estructura almacena el número del hilo (`thread_number`), la cuenta total 
 * de hilos (`thread_count`), un puntero a los datos compartidos (`shared_data`),
 * y el identificador del hilo (`thread_id`).
 */
typedef struct private_data {
    uint64_t thread_number;  ///< Número de hilo (identificador único del hilo).
    uint64_t thread_count;   ///< Número total de hilos creados.
    void* shared_data;       ///< Puntero a los datos compartidos entre hilos.
    pthread_t thread_id;     ///< Identificador del hilo.
    struct private_data* next;  ///< Puntero al siguiente nodo en la lista enlazada.
} private_data_t;

/**
 * @brief Función que ejecuta cada hilo.
 *
 * Esta función es llamada por cada hilo creado. Imprime el número de hilo y 
 * el total de hilos creados.
 *
 * @param data Puntero a los datos privados del hilo (`private_data_t`).
 * @return Siempre retorna NULL.
 */
void* greet(void* data);

/**
 * @brief Crea los hilos y espera a que todos terminen.
 *
 * Esta función reserva memoria para los hilos y sus datos privados, luego 
 * crea cada hilo, asignándole sus datos privados y el número total de hilos.
 * Finalmente, espera a que todos los hilos terminen su ejecución antes de 
 * liberar la memoria reservada.
 *
 * @param thread_count Número de hilos a crear.
 * @return EXIT_SUCCESS si todo salió bien, o un código de error si hubo problemas al crear los hilos.
 */
int create_threads(uint64_t thread_count);


/**
 * @brief Procedimiento principal.
 *
 * Este es el punto de entrada del programa. Crea el número especificado
 * de hilos, cada uno de los cuales imprime un mensaje de saludo.
 *
 * @param argc Número de argumentos de la línea de comandos.
 * @param argv Arreglo de argumentos de la línea de comandos.
 * @return int Estado de salida del programa.
 */
int main(int argc, char* argv[]) {
    int error = EXIT_SUCCESS;
    uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);

    if (argc == 2) {
        if (sscanf(argv[1], "%" SCNu64, &thread_count) != 1) {
            fprintf(stderr, "Error: número de hilos inválido\n");
            return 11;
        }
    }

    error = create_threads(thread_count);
    return error;
}

/**
 * @brief Crea el número especificado de hilos.
 *
 * Esta función crea los hilos y espera a que terminen.
 *
 * @param thread_count Número de hilos a crear.
 * @return int Código de estado que indica éxito o fracaso.
 */
int create_threads(uint64_t thread_count) {
    int error = EXIT_SUCCESS;
    pthread_t* threads = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
    private_data_t* private_data = (private_data_t*) calloc(thread_count, sizeof(private_data_t));

    if (threads && private_data) {
        for (uint64_t thread_number = 0; thread_number < thread_count; ++thread_number) {
            private_data[thread_number].thread_number = thread_number;
            private_data[thread_number].thread_count = thread_count;
            error = pthread_create(&threads[thread_number], NULL, greet, &private_data[thread_number]);
            if (error != EXIT_SUCCESS) {
                fprintf(stderr, "Error: no se pudo crear el hilo secundario\n");
                error = 21;
                break;
            }
        }

        printf("Hola desde el hilo principal\n");

        for (uint64_t thread_number = 0; thread_number < thread_count; ++thread_number) {
            pthread_join(threads[thread_number], NULL);
        }

        free(private_data);
        free(threads);
    } else {
        fprintf(stderr, "Error: no se pudo asignar memoria para %" PRIu64 " hilos\n", thread_count);
        error = 22;
    }

    return error;
}

/**
 * @brief Función de saludo para cada hilo.
 *
 * Esta función es ejecutada por cada hilo e imprime un mensaje de saludo
 * indicando el número de hilo y el total de hilos.
 *
 * @param data Puntero a los datos específicos del hilo (private_data_t).
 * @return void* Siempre retorna NULL.
 */
void* greet(void* data) {
    private_data_t* private_data = (private_data_t*) data;
    printf("Hola desde el hilo secundario %" PRIu64 " de %" PRIu64 "\n", private_data->thread_number, private_data->thread_count);
    return NULL;
}