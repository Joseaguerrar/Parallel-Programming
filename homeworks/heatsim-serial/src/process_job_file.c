//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <process_job_file.h>
#include <stdio.h>
#include <read.h>
#include <matrix_operations.h>
#include <simulation_reader.h>  // Incluir el nuevo header para leer el archivo de simulaciones

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
    // Inicialmente, reservar memoria para un número pequeño de simulaciones
    int capacity = 10;
    SimulationParams *params = malloc(capacity * sizeof(SimulationParams));
    if (!params) {
        perror("Error al asignar memoria");
        return;
    }

    // Leer el archivo de simulaciones y llenar el array de estructuras
    int num_simulations = 0;
    FILE *file = fopen(job_file, "r");
    if (!file) {
        perror("Error al abrir el archivo de simulación");
        free(params);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (num_simulations >= capacity) {
            // Incrementar la capacidad cuando se alcance el límite actual
            capacity *= 2;
            params = realloc(params, capacity * sizeof(SimulationParams));
            if (!params) {
                perror("Error al reasignar memoria");
                fclose(file);
                return;
            }
        }
        // Llenar el parámetro de simulación actual
        sscanf(line, "%s %lf %lf %lf %lf",
               params[num_simulations].plate_file,
               &params[num_simulations].dtime,
               &params[num_simulations].alpha,
               &params[num_simulations].height,
               &params[num_simulations].epsilon);
        num_simulations++;
    }

    fclose(file);  // Cerrar el archivo de simulación después de la lectura

    /* Crear el nombre del archivo de reporte .tsv basado
    en el nombre del archivo de trabajo*/
    char output_file[256];
    snprintf(output_file, sizeof(output_file), "%s/%s.tsv", output_dir,
             strrchr(job_file, '/') ? strrchr(job_file, '/') + 1 : job_file);

    // Abrir archivo de reporte para escritura
    FILE *report_fp = fopen(output_file, "w");
    if (!report_fp) {
        perror("Error al abrir el archivo de reporte");
        free(params);
        return;
    }

    // Procesar cada simulación leída del archivo
    for (int i = 0; i < num_simulations; i++) {
        printf("Procesando Simulación %d:\n", i + 1);
        printf("Archivo: %s\n", params[i].plate_file);
        printf("Δt: %lf\n", params[i].dtime);
        printf("α: %lf\n", params[i].alpha);
        printf("h: %lf\n", params[i].height);
        printf("ε: %lf\n", params[i].epsilon);

        // Leer la matriz inicial y los parámetros desde el archivo binario
        int filas, columnas;
        double **matriz = read_file(params[i].plate_file, &filas, &columnas,
                                    &params[i].epsilon, &params[i].dtime,
                                    &params[i].alpha, &params[i].height);

        // Realizar la simulación de transferencia de calor
        int k = 0;
        double max_change = 0.0;
        do {
            max_change = update_temp(matriz, filas, columnas, params[i].dtime,
            params[i].alpha, params[i].height);
            k++;
        } while (max_change > params[i].epsilon);

        // Crear el nombre del archivo para guardar el estado final de la matriz
        snprintf(output_file, sizeof(output_file), "%s/%s-%d.bin",
        output_dir, strtok(params[i].plate_file, "."), k);
        // Guardar la matriz final en un archivo binario
        print_result(output_file, matriz, filas, columnas);

        // Calcular el tiempo total transcurrido en la simulación
        time_t total_time = k * params[i].dtime;
        // Formatear el tiempo en un formato legible (YYYY/MM/DD hh:mm:ss)
        char formatted_time[48];
        format_time(total_time, formatted_time, sizeof(formatted_time));

        // Escribir los resultados en el archivo de reporte .tsv
        fprintf(report_fp, "%s\t%lf\t%lf\t%lf\t%lf\t%d\t%s\n",
        params[i].plate_file, params[i].dtime, params[i].alpha,
        params[i].height, params[i].epsilon, k, formatted_time);

        // Liberar la memoria asignada para la matriz
        free_matrix(matriz, filas);
    }

    // Cerrar el archivo de reporte después de completar todas las simulaciones
    fclose(report_fp);

    // Liberar la memoria asignada para las simulaciones
    free(params);
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
    const struct tm* gmt = gmtime(&seconds); //NOLINT
    snprintf(text, capacity, "%04d/%02d/%02d\t%02d:%02d:%02d",
             gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday,
             gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
    return text;
}
