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
}