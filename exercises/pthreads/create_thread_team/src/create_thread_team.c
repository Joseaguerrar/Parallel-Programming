// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @file create_thread_team.c
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
 * @param count Número de hilos a crear.
 * @param routine Función que ejecutará cada hilo.
 * @param data Puntero a los datos compartidos que se pasarán a cada hilo.
 * @return private_data_t* Arreglo de estructuras de datos privados, o NULL si ocurre un error.
 */
private_data_t* create_threads(size_t count, void*(*routine)(void*), void* data);

/**
 * @brief Espera a que todos los hilos en el equipo finalicen su ejecución.
 *
 * Esta función se asegura de unir los hilos utilizando el identificador de hilo almacenado en la estructura `private_data_t` y finalmente libera la memoria asignada para las estructuras de datos privados.
 *
 * @param count Número de hilos en el equipo.
 * @param private_data Arreglo de estructuras de datos privados.
 * @return int Número de errores encontrados al unir hilos.
 */
int join_threads(size_t count, private_data_t* private_data);

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

    // Crear hilos
    private_data_t* private_data = create_threads(thread_count, greet, NULL);
    if (private_data == NULL) {
        return EXIT_FAILURE;
    }

    // Mensaje desde el hilo principal
    printf("Hola desde el hilo principal\n");

    // Esperar a que todos los hilos finalicen
    int errors = join_threads(thread_count, private_data);

    return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}

/**
 * @brief Crea un conjunto de hilos que ejecutan una rutina especificada.
 *
 * Esta función crea un número determinado de hilos (`count`), cada uno de los cuales ejecuta la función
 * especificada por el puntero `routine`. Se reserva memoria dinámica para un arreglo de estructuras
 * `private_data_t`, que almacena información específica para cada hilo, como su número de hilo,
 * el número total de hilos, y un puntero a los datos compartidos.
 *
 * Si la creación de algún hilo falla, la función `join_threads` se invoca para limpiar los hilos creados
 * hasta ese punto, y la función retorna `NULL`.
 *
 * @param count Número de hilos a crear.
 * @param routine Puntero a la función que ejecutará cada hilo. Esta función debe seguir la firma `void*(*routine)(void*)`.
 * @param data Puntero a los datos compartidos que se pasarán a cada hilo.
 * @return private_data_t* Puntero al arreglo de estructuras `private_data_t` que contiene los datos privados
 * de cada hilo, o `NULL` si ocurre un error durante la creación de los hilos.
 */
private_data_t* create_threads(size_t count, void*(*routine)(void*), void* data) {
    private_data_t* private_data = (private_data_t*) calloc(count, sizeof(private_data_t));
    if (private_data) {
        for (size_t index = 0; index < count; ++index) {
            private_data[index].thread_number = index;
            private_data[index].thread_count = count;
            private_data[index].shared_data = data;
            if (pthread_create(&private_data[index].thread_id, NULL, routine, &private_data[index]) != 0) {
                fprintf(stderr, "Error: no se pudo crear el hilo %zu\n", index);
                join_threads(index, private_data);
                return NULL;
            }
        }
    }
    return private_data;
}

/**
 * @brief Función que ejecuta cada hilo para mostrar un mensaje de saludo.
 *
 * Esta función es llamada por cada hilo creado. Recibe un puntero a una estructura
 * `private_data_t` que contiene información específica del hilo, como su número de hilo
 * y el número total de hilos. La función imprime un mensaje que incluye el número del hilo
 * y el total de hilos.
 *
 * @param data Puntero a la estructura de datos privados del hilo (`private_data_t`).
 * @return void* Siempre retorna `NULL` al finalizar la ejecución del hilo.
 */
void* greet(void* data) {
    private_data_t* private_data = (private_data_t*) data;
    printf("Hola desde el hilo secundario %" PRIu64 " de %" PRIu64 "\n", private_data->thread_number, private_data->thread_count);
    return NULL;
}

/**
 * @brief Espera a que todos los hilos creados terminen su ejecución y libera la memoria asociada.
 *
 * Esta función recorre un arreglo de estructuras `private_data_t`, utilizando `pthread_join`
 * para esperar a que cada hilo termine su ejecución. Si ocurre un error al unir un hilo, 
 * se incrementa un contador de errores. Al finalizar, la función libera la memoria asignada 
 * al arreglo de estructuras `private_data_t`.
 *
 * @param count Número de hilos en el equipo.
 * @param private_data Puntero al arreglo de estructuras de datos privados de cada hilo.
 * @return int Número de errores ocurridos durante la unión de los hilos. Si todos los hilos
 * se unen correctamente, retorna 0.
 */
int join_threads(size_t count, private_data_t* private_data) {
    int error_count = 0;
    for (size_t index = 0; index < count; ++index) {
        const int error = pthread_join(private_data[index].thread_id, NULL);
        if (error) {
            fprintf(stderr, "Error: no se pudo unir el hilo %zu\n", index);
            ++error_count;
        }
    }
    free(private_data);
    return error_count;
}