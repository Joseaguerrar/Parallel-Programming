//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <process_job_file.h>
#include <stdio.h>
#include <read.h>
#include <matrix_operations.h>

/**
 * @brief Procesa un archivo de trabajo y ejecuta simulaciones basadas en sus especificaciones.
 * 
 * Esta función lee un archivo de trabajo (por ejemplo, job001.txt) que contiene una lista de simulaciones 
 * a realizar. Para cada simulación, se lee la matriz inicial desde un archivo binario, se ejecuta la 
 * simulación hasta que se alcanza el equilibrio térmico, y luego se guarda el resultado en un archivo 
 * binario y se escribe un reporte en un archivo .tsv.
 * 
 * @param job_file Ruta al archivo de trabajo que contiene las simulaciones a realizar.
 * @param output_dir Directorio donde se guardarán los archivos de salida (resultados y reporte).
 */
void process_job_file(const char *job_file, const char *output_dir) {
    char line[256];       // Buffer para leer cada línea del archivo de trabajo
    char plate_file[64];  // Nombre del archivo que contiene la matriz inicial
    double dtime, alpha, height, epsilon;  // Parámetros de simulación: Δt, α, h, ε
    int filas, columnas, k;                // Dimensiones de la matriz y contador de iteraciones
    double max_change;     // Variable para almacenar el cambio máximo en cada iteración
    char output_file[256]; // Buffer para almacenar la ruta del archivo de salida
}