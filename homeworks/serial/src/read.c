#include <read.h>
#include <matrix_operations.h>
#include <stdio.h>
#include <stdlib.h>

//implementación de función para leer archivo
double ** read_file(const char *file, int *filas, int *columnas, double *epsilon, 
double *dtime, double *alpha, double *height) {
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
}