//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef HEAT_SIMULATION_H
#define HEAT_SIMULATION_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

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
 * @brief Estructura compartida entre hilos para sincronización y datos comunes.
 */
typedef struct {
    bool balance_point; /**< Indica si se ha alcanzado el equilibrio térmico. */
    double** global_matrix; /**< Matriz global actualizada por los hilos. */
    const double* coef; /**< Coeficiente precalculado para la simulación. */
} shared_data;
/**
 * @brief Estructura para pasar datos a cada hilo de simulación.
 */
typedef struct {
    double** local_matrix;   /**< Matriz local */
    uint64_t start_row;   /**< Fila de inicio asignada al hilo para procesar. */
    uint64_t end_row;        /**< Fila de fin asignada al hilo para procesar. */
    uint64_t columns;        /**< Número de columnas de la matriz. */
    uint64_t rows;           /**< Número de filas de la matriz. */
    double delta_t;          /**< Diferencial de tiempo. */
    double alpha;            /**< Difusividad térmica. */
    double h;                /**< Tamaño de las celdas. */
    double epsilon;          /**< Sensibilidad del punto de equilibrio. */
    int id;                  /**< ID del hilo para identificarlo */
    const double* local_coef;      /**< Puntero a la estructura compartida */
    shared_data* shared;     /**< Puntero a la estructura compartida */
} private_data;

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
 */
void read_bin_plate(const char* folder,
                    params_matrix* variables_formula,
                    uint64_t lines,
                    const char* jobName,
                    int num_threads);

/**
 * @brief Realiza la simulación de transferencia de calor en una matriz.
 * 
 * @param matrix Matriz de la lámina con los datos iniciales.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @param delta_t Diferencial de tiempo.
 * @param alpha Difusividad térmica.
 * @param h Tamaño de las celdas.
 * @param epsilon Sensitividad del punto de equilibrio.
 * @param num_threads Cantidad de hilos 
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
                                    int num_threads);

/**
 * @brief Función ejecutada por cada hilo durante la simulación de transferencia de calor.
 * 
 * Cada hilo procesa un subconjunto de filas de la matriz y actualiza el estado térmico
 * de esas celdas.
 * 
 * @return NULL.
 */
void* heat_transfer_simulation_thread(void* arg);

/**
 * @brief Formatea un tiempo dado en segundos a un formato legible.
 * 
 * @param seconds Tiempo en segundos a formatear.
 * @param text Buffer donde se almacenará el tiempo formateado.
 * @param capacity Capacidad del buffer `text`.
 * 
 * @return Un puntero al buffer `text` que contiene el tiempo formateado.
 */

/**
 * @brief Crea una matriz vacía de tamaño especificado.
 * 
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 * @return Puntero a la matriz creada. Retorna NULL si la asignación de memoria falla.
 */
double** create_empty_matrix(uint64_t rows, uint64_t columns);

/**
 * @brief Copia los datos de una matriz a otra.
 * 
 * @param dest_matrix Puntero a la matriz destino donde se copiarán los datos.
 * @param src_matrix Puntero a la matriz fuente desde donde se copiarán los datos.
 * @param rows Número de filas de ambas matrices.
 * @param columns Número de columnas de ambas matrices.
 */
void copy_matrix(double** dest_matrix,
                 double** src_matrix,
                 uint64_t rows,
                 uint64_t columns);

/**
 * @brief Libera la memoria asignada a una matriz.
 * 
 * @param matrix Puntero a la matriz cuya memoria se va a liberar.
 * @param rows Número de filas de la matriz.
 */
void free_matrix(double** matrix, uint64_t rows);

/**
 * @brief Imprime el contenido de una matriz de tamaño dado.
 * 
 * @param matrix Matriz que se desea imprimir.
 * @param rows Número de filas de la matriz.
 * @param columns Número de columnas de la matriz.
 */
void print_matrix(double** matrix, uint64_t rows, uint64_t columns);

char* format_time(const time_t seconds, char* text, const size_t capacity);

/**
 * @brief Genera el archivo de reporte (.tsv) con los resultados de la simulación.
 * 
 */
void generate_report_file(const char* folder,
                        const char* jobName,
                        params_matrix* variables_formula,
                        uint64_t* states_k,
                        uint64_t lines);

/**
 * @brief Genera un archivo binario con el estado final de la matriz después de la simulación.
 */
void generate_bin_file(double** matrix,
                        uint64_t rows,
                        uint64_t columns,
                        const char* folder,
                        const char* jobName,
                        uint64_t states_k);

#endif  //  HEAT_SIMULATION_H
