//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>

#include "reader.h"
#include <stdio.h>
#include <stdlib.h>

// Lee el archivo de texto y extrae los parámetros de simulación para cada placa
int read_simulation_file(const char *filename, SimulationParams *params, int max_params) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int count = 0;
    while (count < max_params && fscanf(file, "%s %lf %lf %lf %lf",
        params[count].plate_file,
        &params[count].dtime,
        &params[count].alpha,
        &params[count].height,
        &params[count].epsilon) == 5) {
        count++;
    }

    fclose(file);
    return count;
}