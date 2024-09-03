#include <read.h>
#include <matrix_operations.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]){
    if (argc<2)
    {
        fprintf(stderr, "Uso: %s <archivo de entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file= argv[1];

    int filas, columnas;
    double epsilon, dtime, alpha, height;

    double **matriz= read_file(input_file, &filas, &columnas, &epsilon, &dtime, &alpha, &height);
    
    int k=0;
    double max_change;

    while (max_change> epsilon)
    {
        max_change=update_temp(matriz, filas, columnas, dtime,alpha,height,epsilon);
        k++;
    }
    
    printf("Equilibrio alcanzado en %d iteraciones.\n", k);
}