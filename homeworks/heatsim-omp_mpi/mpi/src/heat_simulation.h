//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef HEAT_SIMULATION_H
#define HEAT_SIMULATION_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

/**
 * @brief Estructura para almacenar los parámetros de cada simulación.
 */
typedef struct {
    char* filename;       /**< Nombre del archivo binario. */
    double delta_t;       /**< Diferencial de tiempo. */
    double alpha;         /**< Difusividad térmica. */
    double h;             /**< Tamaño de las celdas. */
    double epsilon;       /**< Sensitividad del punto de equilibrio. */
} params_matrix;

/**
 * @brief Lee el archivo de trabajo (.txt) y extrae los parámetros de simulación para cada placa.
 * 
 * @param jobName Nombre del archivo de trabajo.
 * @param folder Carpeta donde se encuentra el archivo de trabajo.
 * @param lines Puntero a una variable donde se almacenará la cantidad de líneas en el archivo.
 * 
 * @return Un puntero a un arreglo de estructuras `params_matrix` que contiene los parámetros de simulación para cada placa.
 * @return NULL si hay un error al leer el archivo o al asignar memoria.
 */
params_matrix* read_job_txt(const char* jobName,
                            const char* folder,
                            uint64_t* lines);

/**
 * @brief Cuenta el número de líneas en un archivo de texto.
 * 
 * @param fileName Nombre del archivo a leer.
 * 
 * @return El número de líneas en el archivo.
 * @return 0 si no se pudo abrir el archivo.
 */
uint64_t count_lines(const char* fileName);

/**
 * @brief Lee el archivo binario correspondiente a cada lámina y ejecuta la simulación de transferencia de calor.
 * 
 * @param folder Carpeta donde se encuentran los archivos binarios.
 * @param variables_formula Arreglo de estructuras `params_matrix` que contiene los parámetros de cada simulación.
 * @param lines Número de simulaciones a realizar.
 * @param jobName Nombre del archivo de trabajo.
 */
void read_bin_plate(const char* folder,
                    params_matrix* variables_formula,
                    uint64_t lines,
                    const char* jobName);

/**
 * @brief Realiza la simulación de transferencia de calor en una matriz distribuida entre varios procesos.
 * @param matrix Matriz de la lámina con los datos iniciales.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @param delta_t Diferencial de tiempo.
 * @param alpha Difusividad térmica.
 * @param h Tamaño de las celdas.
 * @param epsilon Sensitividad del punto de equilibrio.
 * @param rank Rango del proceso.
 * @param size Número total de procesos.
 * 
 * @return Número de estados hasta alcanzar el punto de equilibrio.
 */
uint64_t heat_transfer_simulation(double** matrix,
                                    uint64_t rows,
                                    uint64_t columns,
                                    double delta_t,
                                    double alpha,
                                    double h,
                                    double epsilon,
                                    int rank,
                                    int size);

// Intercambia las filas frontera entre procesos vecinos
void exchange_borders(double* local_matrix, uint64_t local_rows, uint64_t columns, int rank, int size);
/**
 * @brief Crea una matriz vacía de tamaño dado.
 * 
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @return Un puntero a la matriz vacía.
 */
double** create_empty_matrix(uint64_t rows, uint64_t columns);

/**
 * @brief Copia el contenido de una matriz a otra.
 * 
 * @param dest_matrix Matriz de destino.
 * @param src_matrix Matriz fuente.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 */
void copy_matrix(double** dest_matrix, double** src_matrix,
                                               uint64_t rows, uint64_t columns);

/**
 * @brief Libera la memoria de una matriz.
 * 
 * @param matrix Matriz a liberar.
 * @param rows Número de filas de la matriz.
 */
void free_matrix(double** matrix, uint64_t rows);

/**
 * @brief Imprime el contenido de una matriz en la consola.
 * 
 * @param matrix Matriz a imprimir.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 */
void print_matrix(double** matrix, uint64_t rows, uint64_t columns);

/**
 * @brief Formatea un tiempo dado en segundos a un formato legible.
 * 
 * @param seconds Tiempo en segundos a formatear.
 * @param text Buffer donde se almacenará el tiempo formateado.
 * @param capacity Capacidad del buffer `text`.
 * 
 * @return Un puntero al buffer `text` que contiene el tiempo formateado.
 */
char* format_time(const time_t seconds, char* text, const size_t capacity);

/**
 * @brief Genera el archivo de reporte (.tsv) con los resultados de la simulación.
 * 
 * @param folder Carpeta donde se guardará el archivo de reporte.
 * @param jobName Nombre del archivo de trabajo.
 * @param variables_formula Arreglo de estructuras `params_matrix` que contiene los parámetros de la simulación.
 * @param state_k Arreglo que contiene los estados finales de cada simulación.
 * @param lines Número de líneas (simulaciones) en el archivo de trabajo.
 */
void generate_report_file(const char* folder,
                        const char* jobName,
                        params_matrix* variables_formula,
                        uint64_t* state_k,
                        uint64_t lines);

/**
 * @brief Genera un archivo binario con el estado final de la matriz después de la simulación.
 * 
 * @param matrix Matriz final después de la simulación.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @param folder Carpeta donde se guardará el archivo.
 * @param jobName Nombre del archivo de trabajo.
 * @param state_k Número de estados alcanzados hasta el equilibrio.
 */
void generate_bin_file(double** matrix,
                        uint64_t rows,
                        uint64_t columns,
                        const char* folder,
                        const char* jobName,
                        uint64_t state_k);

#endif  //  HEAT_SIMULATION_H
