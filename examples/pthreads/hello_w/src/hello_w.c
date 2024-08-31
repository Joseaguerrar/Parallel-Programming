// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* greet(void* data);
int create_threads(uint64_t thread_count);

int main(int argc, char* argv[]) {
    uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);  // Cantidad de CPUs disponibles por defecto

    // Si se pasa un argumento, se usa como la cantidad de hilos a crear
    if (argc == 2) {
        if (sscanf(argv[1], "%" SCNu64, &thread_count) != 1 || thread_count < 1) {
            fprintf(stderr, "Error: invalid thread count\n");
            return 1;
        }
    }

    // Crea los hilos
    int error = create_threads(thread_count);
    return error;
}

// Crea la cantidad especificada de hilos
int create_threads(uint64_t thread_count) {
    pthread_t* threads = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
    if (!threads) {
        fprintf(stderr, "Error: could not allocate memory for threads\n");
        return 1;
    }

    // Crear los hilos
    for (uint64_t i = 0; i < thread_count; ++i) {
        uint64_t* arg = malloc(sizeof(uint64_t));  // Almacenar el número de hilo para pasarlo al hilo
        if (!arg) {
            fprintf(stderr, "Error: could not allocate memory for thread argument\n");
            free(threads);
            return 1;
        }
        *arg = i;

        if (pthread_create(&threads[i], NULL, greet, arg) != 0) {
            fprintf(stderr, "Error: could not create thread %" PRIu64 "\n", i);
            free(arg);
            free(threads);
            return 1;
        }
    }

    // Esperar a que todos los hilos terminen
    for (uint64_t i = 0; i < thread_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}

// Función que se ejecuta en cada hilo
void* greet(void* data) {
    uint64_t thread_number = *(uint64_t*)data;  // Convertir el argumento al número de hilo
    printf("Hello from secondary thread %" PRIu64 "\n", thread_number);
    free(data);  // Liberar la memoria asignada para el argumento
    return NULL;
}