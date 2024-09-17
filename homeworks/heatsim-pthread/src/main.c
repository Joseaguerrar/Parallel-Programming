//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
        printf("Uso: %s <carpeta> <archivo de trabajo>\n", argv[0]);
        return 1;
    }

    const char *folder = argv[1];
    const char *jobName = argv[2];

    // Leer el archivo de trabajo
    uint64_t lines;
    params_matrix* variables = read_job_txt(jobName, folder, &lines);
    if (!variables) {
        fprintf(stderr, "Error al leer el archivo de trabajo.\n");
        return 1;
    }

    // Simulación de transferencia de calor
    read_bin_plate(folder, variables, lines, jobName);

    // Liberar memoria
    for (uint64_t i = 0; i < lines; i++) {
        free(variables[i].filename);
    }
    free(variables);

    printf("Simulación completada.\n");
    return 0;
}
