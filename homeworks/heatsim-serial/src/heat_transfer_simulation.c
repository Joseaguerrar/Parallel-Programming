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
        uint64_t states_k = heat_transfer_simulation(matrix, rows, columns, delta_t, alpha, h, epsilon);
        array_state_k[i] = states_k;
        generate_bin_file(matrix, rows, columns, folder, variables[i].filename, states_k);

        for (uint64_t i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
        fclose(bin_file);
    }
    generate_report_file(folder,jobName,variables,array_state_k,lines);
    free(array_state_k);
    
}

// Implementación de simulación de transferencia de calor
uint64_t heat_transfer_simulation(double** matrix, uint64_t rows, uint64_t columns, double delta_t, double alpha, double h, double epsilon){
    bool balance_point=false;
    uint64_t states_k = 0;

    double** temp_matrix = malloc(rows * sizeof(double*));
    for (uint64_t i = 0; i < rows; i++) {
        temp_matrix[i] = malloc(columns * sizeof(double));
    }

    while (!balance_point)
    {
        balance_point=true;
        states_k++;
        for (uint64_t i = 1; i < rows - 1; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                double actual_temperature = matrix[i][j];
                double new_temperature = actual_temperature + ((delta_t * alpha) / (h * h)) * (matrix[i-1][j] + matrix[i+1][j] + matrix[i][j-1] + matrix[i][j+1] - 4 * actual_temperature);
                temp_matrix[i][j] = new_temperature;
                if (fabs(new_temperature - actual_temperature) > epsilon) {
                    balance_point = false;
                }
            }
        }

        for (uint64_t i = 1; i < rows - 1; i++) {
            for (uint64_t j = 1; j < columns - 1; j++) {
                matrix[i][j] = temp_matrix[i][j];
            }
        }
    }

    for (uint64_t i = 0; i < rows; i++) {
        free(temp_matrix[i]);
    }
    free(temp_matrix);
    return states_k;
}

