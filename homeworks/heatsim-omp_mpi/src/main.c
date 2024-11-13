//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>  // Para obtener el número de CPUs (núcleos) disponibles
#include <time.h>    // Para la función clock_gettime
#include "heat_simulation.h"

/**
 * @brief Programa principal para ejecutar la simulación de transferencia de calor.
 * 
 * @param argc Número de argumentos pasados a la línea de comandos.
 * @param argv Arreglo de cadenas que contiene los argumentos pasados a la línea de comandos.
 * 
 * @return 0 si la simulación se ejecuta correctamente, 1 si hay un error.
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <carpeta> <archivo de trabajo> [num_hilos]\n", argv[0]);
        return 1;
    }

    const char *folder = argv[1];
    const char *jobName = argv[2];

    // Determinar el número de hilos
    int num_threads;
    if (argc >= 4) {
        num_threads = atoi(argv[3]);  // Convertir argumento a entero
        if (num_threads <= 0) {
            fprintf(stderr,
             "Número de hilos inválido. Usando número de CPUs disponibles.\n");
            num_threads = sysconf(_SC_NPROCESSORS_ONLN);
            // Obtener núcleos de la máquina
        }
    } else {
        num_threads = sysconf(_SC_NPROCESSORS_ONLN);
        // Obtener núcleos de la máquina si no se proporciona el argumento
    }

    printf("Número de hilos a utilizar: %d\n", num_threads);

    // Iniciar el reloj para medir el tiempo
    struct timespec start_time, finish_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Leer el archivo de trabajo
    uint64_t lines;
    params_matrix* variables = read_job_txt(jobName, folder, &lines);
    if (!variables) {
        fprintf(stderr, "Error al leer el archivo de trabajo.\n");
        return 1;
    }

    // Simulación de transferencia de calor
    read_bin_plate(folder, variables, lines, jobName, num_threads);

    // Medir el tiempo después de completar la simulación
    clock_gettime(CLOCK_MONOTONIC, &finish_time);

    // Calcular el tiempo transcurrido
    double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
                     (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
    printf("Tiempo de ejecución: %.9lfs\n", elapsed);

    // Liberar memoria
    for (uint64_t i = 0; i < lines; i++) {
        free(variables[i].filename);
    }
    free(variables);

    printf("Simulación completada.\n");
    return 0;
}
