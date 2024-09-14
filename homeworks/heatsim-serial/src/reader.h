//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef READER_H
#define READER_H

#define MAX_LINE_LENGTH 256
#define DEFAULT_EPSILON 0.0

/**
 * @file reader.h
 * @brief Funciones para la entrada/salida de datos y lectura de parámetros de simulación.
 */

/**
 * @brief Estructura para almacenar los parámetros de simulación.
 */
typedef struct {
    char plate_file[64];  // Nombre del archivo binario
    double dtime;         // Diferencial de tiempo (Δt)
    double alpha;         // Difusividad térmica (α)
    double height;        // Tamaño de las celdas (h)
    double epsilon;       // Parámetro de equilibrio térmico (ε)
} SimulationParams;

/**
 * @brief Lee el archivo de texto y extrae los parámetros de simulación para cada placa.
 * 
 * @param filename Nombre del archivo de texto que contiene los parámetros.
 * @param params Array de estructuras `SimulationParams` para almacenar los parámetros leídos.
 * @param max_params Número máximo de simulaciones que puede almacenar el array `params`.
 * @return int Número de simulaciones leídas desde el archivo.
 */
int read_simulation_file(const char *filename,
                        SimulationParams *params, int max_params);

/**
 * @brief Lee los datos desde un archivo y almacena la matriz de temperaturas y parámetros de simulación.
 * 
 * @return double** Un puntero doble que apunta a la matriz de temperaturas.
 */
double ** read_file(const char *file, int *filas, int *columnas,
double *epsilon, double *dtime, double *alpha, double *height);

/**
 * @brief Imprime el resultado de la simulación en un archivo.
 * 
 */
void print_result(const char *file, double **matriz, int filas, int columnas);

#endif  //  READER_H
