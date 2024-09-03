//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <read.h>
#include <matrix_operations.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Lee una matriz y los parámetros de simulación desde un archivo binario.
 *
 * Esta función abre un archivo binario y lee los datos necesarios para
 * la simulación, incluyendo el tamaño de la matriz, sus valores, y los 
 * parámetros de simulación como epsilon, dtime, alpha y height.
 *
 * @param file Nombre del archivo binario de entrada.
 * @param filas Puntero a un entero donde se almacenará el número de filas.
 * @param columnas Puntero a un entero donde se almacenará el número de columnas.
 * @param epsilon Puntero a un double donde se almacenará el valor de epsilon.
 * @param dtime Puntero a un double donde se almacenará el valor de dtime.
 * @param alpha Puntero a un double donde se almacenará el valor de alpha.
 * @param height Puntero a un double donde se almacenará el valor de height.
 * 
 * @return Un puntero doble a la matriz de double leída desde el archivo.
 */
double ** read_file(const char *file, int *filas, int *columnas,
double *epsilon, double *dtime, double *alpha, double *height) {
    // Abrimos el archivo, si es nulo, tira error
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Leemos el número de filas y columnas desde el archivo
    fread(filas, sizeof(int), 1, fp);
    fread(columnas, sizeof(int), 1, fp);

    // Creamos la matriz con los valores obtenidos
    double **matriz = asign_matrix(*filas, *columnas);

    // Leemos los valores de la matriz desde el archivo
    for (int i = 0; i < *filas; i++) {
        fread(matriz[i], sizeof(double), *columnas, fp);
    }

    // Leer los demás parámetros desde el archivo
    fread(epsilon, sizeof(double), 1, fp);
    fread(dtime, sizeof(double), 1, fp);
    fread(alpha, sizeof(double), 1, fp);
    fread(height, sizeof(double), 1, fp);

    // Cerramos el archivo y retornamos la matriz
    fclose(fp);
    return matriz;
}

/**
 * @brief Escribe los resultados de la matriz en un archivo binario.
 *
 * Esta función guarda la matriz de temperaturas en un archivo binario,
 * incluyendo el número de filas y columnas al inicio del archivo.
 *
 * @param file Nombre del archivo de salida donde se guardará la matriz.
 * @param matriz Puntero doble a la matriz de double que se va a guardar.
 * @param filas Número de filas en la matriz.
 * @param columnas Número de columnas en la matriz.
 */
void print_result(const char *file, double **matriz, int filas, int columnas) {
    //  Abrimos el archivo, si es nulo, tira error
    FILE *fp = fopen(file, "wb");
    if (!fp) {
        perror("Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }

    //  Escribimos el número de filas y columnas en el archivo
    fwrite(&filas, sizeof(int), 1, fp);
    fwrite(&columnas, sizeof(int), 1, fp);

    // Escribimos los datos de la matriz en el archivo
    for (int i = 0; i < filas; i++) {
        fwrite(matriz[i], sizeof(double), columnas, fp);
    }

    // Cerramos el archivo
    fclose(fp);
}
