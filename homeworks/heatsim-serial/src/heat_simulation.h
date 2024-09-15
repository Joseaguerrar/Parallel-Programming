//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef HEAT_SIMULATION_H
#define HEAT_SIMULATION_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

// Estructura para almacenar los parámetros de cada simulación
typedef struct {
    char* filename;       // Nombre del archivo binario
    double delta_t;       // Diferencial de tiempo
    double alpha;         // Difusividad térmica
    double h;             // Tamaño de las celdas
    double epsilon;       // Sensitividad del punto de equilibrio
} formula_p;

// Funciones para leer los archivos de simulación
formula_p* leer_job_txt(const char* jobName, const char* carpeta, uint64_t* lineas);
uint64_t contar_lineas(const char* fileName);

// Funciones para la simulación de transferencia de calor
void leer_lamina_bin(const char* carpeta, formula_p* variables_formula, uint64_t lineas, const char* jobName);
uint64_t simulacion_transferencia_calor(double** matriz, uint64_t filas, uint64_t columnas, double delta_t, double alpha, double h, double epsilon);
char* format_time(const time_t seconds, char* text, const size_t capacity);

// Funciones para generar archivos de salida
void generar_archivo_reporte(const char* carpeta, const char* jobName, formula_p* variables_formula, uint64_t* estados_k, uint64_t lineas);
void generar_archivo_bin(double** matriz, uint64_t filas, uint64_t columnas, const char* carpeta, const char* jobName, uint64_t estados_k);

#endif // HEAT_SIMULATION_H