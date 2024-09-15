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
            fprintf(stderr,"Error al asignar memoria para las filas de la matriz\n");
            fclose(bin_file);
            return;
        }
        
        for (uint64_t i = 0; i < rows; i++)
        {
            matrix[i]=malloc(columns * sizeof(double));
            if (matrix[i]==NULL)
            {
                fprintf(stderr, "Error al asignar memoria para las columnas de la matriz\n");
                for (uint64_t j = 0; j < i; j++)
                {
                    free(matrix);
                }
                free(matrix);
                fclose(bin_file);
                return;
                
            }
            
        }
        double delta_t = variables[i].delta_t;
        double alpha = variables[i].alpha;
        double h = variables[i].h;
        double epsilon = variables[i].epsilon;
        uint64_t states_k = simulacion_transferencia_calor(matrix, rows, columns, delta_t, alpha, h, epsilon);
        array_state_k[i] = states_k;
        generar_archivo_bin(matrix, rows, columns, folder, variables[i].filename, states_k);

        for (uint64_t i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
        fclose(bin_file);
    }
    
}

