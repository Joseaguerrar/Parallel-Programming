//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <read.h>
#include <matrix_operations.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]) {
    //  Verificar que se haya pasado el nombre del archivo de entrada
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo de entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];

    int filas, columnas;
    double epsilon, dtime, alpha, height;

    //  Leer la matriz y parámetros de simulación desde el archivo binario
    double **matriz = read_file(input_file, &filas, &columnas,
    &epsilon, &dtime, &alpha, &height);

    int k = 0;
    double max_change = 0.0;
    //  Realizar iteraciones hasta que el cambio máximo sea menor que epsilon
    while (max_change> epsilon) {
        max_change = update_temp(matriz, filas, columnas, dtime, alpha, height);
        k++;
    }
    //  Mostrar el número de iteraciones hasta alcanzar el equilibrio
    printf("Equilibrio alcanzado en %d iteraciones.\n", k);

    /* Preparar el nombre del archivo de salida 
    basado en el número de iteraciones*/
    char output_file[256];
    snprintf(output_file, sizeof(output_file), "output_plate_%d.bin", k);

    // Guardar la matriz de temperaturas final en un archivo binario
    print_result(output_file, matriz, filas, columnas);

    // Liberar la memoria asignada para la matriz
    free_matrix(matriz, filas);
    return EXIT_SUCCESS;
}  // endprocedure
