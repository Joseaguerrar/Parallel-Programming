#ifndef READ_H
#define READ_H
//Funciones para la entrada/salida
double ** read_file(const char *file, int *filas, int *columnas, double *epsilon, 
double *dtime, double *alpha, double *height);
//Función para imprimir el resultado
void print_result(const char *file, double **matriz, int filas, int columnas);


#endif //READ_H