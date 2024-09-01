#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H
//Función para la simulación de transferencia de calor y sus argumentos
double update_temp(double **matriz, int filas, int columnas, double dtime,
double alpha, double height, double epsilon);
//Función para crear la matriz
double** asign_matrix(int filas, int columnas);
//Función para liberar espacio de matriz
void free_matrix(double **matriz, int filas);
#endif //MATRIX_OPERATIONS_H