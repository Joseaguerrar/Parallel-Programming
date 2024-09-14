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
// Lee los datos desde un archivo y almacena la matriz de temperaturas y parámetros de simulación
double ** read_file(const char *file, int *filas, int *columnas, double *epsilon, double *dtime, double *alpha, double *height) {
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    fread(filas, sizeof(int), 1, fp);
    fread(columnas, sizeof(int), 1, fp);

    double **matriz = asign_matrix(*filas, *columnas);

    for (int i = 0; i < *filas; ++i) {
        fread(matriz[i], sizeof(double), *columnas, fp);
    }

    fclose(fp);
    return matriz;
}
// Imprime el resultado de la simulación en un archivo
void print_result(const char *file, double **matriz, int filas, int columnas) {
    FILE *fp = fopen(file, "wb");
    if (!fp) {
        perror("Error al abrir el archivo para escritura");
        return;
    }

    fwrite(&filas, sizeof(int), 1, fp);
    fwrite(&columnas, sizeof(int), 1, fp);

    for (int i = 0; i < filas; ++i) {
        fwrite(matriz[i], sizeof(double), columnas, fp);
    }

    fclose(fp);
}
