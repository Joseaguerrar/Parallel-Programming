#include <read.h>
#include <matrix_operations.h>
#include <stdio.h>
#include <stdlib.h>

//implementación de función para leer archivo
double ** read_file(const char *file, int *filas, int *columnas, double *epsilon, 
double *dtime, double *alpha, double *height) {
    //Abrimos el archivo, si es nulo, tira error
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    //Para leer archivos binarios
    fread(filas, sizeof(int), 1, fp);
    fread(columnas, sizeof(int), 1, fp);
    
    //creamos la matriz con los valores obtenidos
    double **matriz = asign_matrix(*filas, *columnas);

    for (int i = 0; i < *filas; i++) {
        fread(matriz[i], sizeof(double), *columnas, fp);
    }

    // Leer los demás parámetros desde el archivo
    fread(epsilon, sizeof(double), 1, fp);
    fread(dtime, sizeof(double), 1, fp);
    fread(alpha, sizeof(double), 1, fp);
    fread(height, sizeof(double), 1, fp);

    //Cerramos el archivo y retornamos la matriz
    fclose(fp);
    return matriz;
}

//Implementación de la función para escribir resultado
void print_result(const char *file, double **matriz, int filas, int columnas) {
    //Abrimos el archivo, si es nulo, tira error
    FILE *fp = fopen(file, "wb");
    if (!fp) {
        perror("Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }

    fwrite(&filas, sizeof(int), 1, fp);
    fwrite(&columnas, sizeof(int), 1, fp);

    for (int i = 0; i < filas; i++) {
        fwrite(matriz[i], sizeof(double), columnas, fp);
    }
    //Cerramos el archivo
    fclose(fp);
}