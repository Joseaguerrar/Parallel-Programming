//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "heat_simulation.h"
void read_bin_plate(const char* folder, params_matrix* variables, uint64_t lines, const char* jobName){
    FILE *bin_file;
    uint64_t rows, columns;
    char direction[512];
    uint64_t* array_state_k=malloc(lines * sizeof(uint64_t));

    for (uint64_t i = 0; i < lines; i++) {
        snprintf(direction, sizeof(direction), "%s/%s", folder, variables[i].filename);
        bin_file = fopen(direction, "rb");
        if (bin_file == NULL) {
            fprintf(stderr, "No se pudo abrir el archivo binario %s\n", variables[i].filename);
            return;
        }

        fread(&rows, sizeof(uint64_t), 1, bin_file);
        fread(&columns, sizeof(uint64_t), 1, bin_file);

        double **matrix =malloc(rows*sizeof(double*));

    if (matrix==NULL)
    {
        fprintf(stderr,"Error al asignar memoria para la matriz\n");
        fclose(bin_file);
        return;
    }
    

}

