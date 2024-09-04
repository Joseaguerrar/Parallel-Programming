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
    //  Buffer para leer cada línea del archivo de trabajo
    char line[256];
    //  Nombre del archivo que contiene la matriz inicial
    char plate_file[64];
    //  Parámetros de simulación: Δt, α, h, ε
    double dtime, alpha, height, epsilon;
    //  Dimensiones de la matriz y contador de iteraciones
    int filas, columnas, k;
    //  Variable para almacenar el cambio máximo en cada iteración
    double max_change;
    //  Buffer para almacenar la ruta del archivo de salida
    char output_file[256];

    // Abrir el archivo de trabajo para lectura
    FILE *fp = fopen(job_file, "r");
    /*Crear el nombre del archivo de reporte .tsv
    basado en el nombre del archivo de trabajo*/
    snprintf(output_file, sizeof(output_file), "%s/%s.tsv", output_dir,
             strrchr(job_file, '/') ? strrchr(job_file, '/') + 1 : job_file);
    // Abrir archivo de reporte para escritura
    FILE *report_fp = fopen(output_file, "w");
    // Verificar que ambos archivos se abrieron correctamente
    if (!fp || !report_fp) {
        perror("Error al abrir el archivo de trabajo o el archivo de reporte");
        exit(EXIT_FAILURE);
    }

    // Leer cada línea del archivo de trabajo
    while (fgets(line, sizeof(line), fp)) {
        // Extraer los parámetros de simulación de la línea leída
        sscanf(line, "%s %lf %lf %lf %lf", plate_file, &dtime,
        &alpha, &height, &epsilon);

        // Crear la ruta completa para el archivo de la matriz inicial
        char plate_path[256];
        snprintf(plate_path, sizeof(plate_path), "%s/%s",
        output_dir, plate_file);
        // Leer la matriz inicial y los parámetros desde el archivo binario
        double **matriz = read_file(plate_path, &filas, &columnas,
        &epsilon, &dtime, &alpha, &height);
        // Realizar la simulación de transferencia de calor
        k = 0;
        max_change = 0.0;
        // Continuar iterando hasta que el cambio máximo sea menor que ε
        while (max_change > epsilon) {
            max_change = update_temp(matriz, filas, columnas,
            dtime, alpha, height);
            k++;
        }
        // Crear el nombre del archivo para guardar el estado final de la matriz
        snprintf(output_file, sizeof(output_file), "%s/%s-%d.bin",
        output_dir, strtok(plate_file, "."), k);
        // Guardar la matriz final en un archivo binario
        print_result(output_file, matriz, filas, columnas);

        // Calcular el tiempo total transcurrido en la simulación
        time_t total_time = k * dtime;
        // Formatear el tiempo en un formato legible (YYYY/MM/DD hh:mm:ss)
        char formatted_time[48];
        format_time(total_time, formatted_time, sizeof(formatted_time));
        // Escribir los resultados en el archivo de reporte .tsv
        fprintf(report_fp, "%s\t%lf\t%lf\t%lf\t%lf\t%d\t%s\n", plate_file,
        dtime, alpha, height, epsilon, k, formatted_time);
        // Liberar la memoria asignada para la matriz
        free_matrix(matriz, filas);
    }
    // Cerrar los archivos después de completar todas las simulaciones
    fclose(fp);
    fclose(report_fp);
}

/**
 * @brief Convierte un tiempo en segundos a un formato legible (YYYY/MM/DD hh:mm:ss).
 * 
 * @param seconds Tiempo en segundos.
 * @param text Buffer donde se almacenará el tiempo formateado.
 * @param capacity Tamaño del buffer `text`.
 * @return char* El mismo puntero `text`, que contiene el tiempo formateado.
 */
char* format_time(const time_t seconds, char* text, const size_t capacity) {
    const struct tm* gmt = gmtime(&seconds); //  NOLINT
    snprintf(text, capacity, "%04d/%02d/%02d\t%02d:%02d:%02d",
    gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday,
    gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
    return text;
}
