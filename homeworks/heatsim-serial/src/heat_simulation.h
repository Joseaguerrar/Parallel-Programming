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
} params_matrix;

// Funciones para leer los archivos de simulación
params_matrix* read_job_txt(const char* jobName, const char* folder, uint64_t* lines);
uint64_t count_lines(const char* fileName);

// Funciones para la simulación de transferencia de calor
void read_bin_plate(const char* folder, params_matrix* variables_formula, uint64_t lines, const char* jobName);
uint64_t heat_transfer_simulation(double** matrix, uint64_t rows, uint64_t columns, double delta_t, double alpha, double h, double epsilon);
char* format_time(const time_t seconds, char* text, const size_t capacity);

// Funciones para generar archivos de salida
void generate_report_file(const char* folder, const char* jobName, params_matrix* variables_formula, uint64_t* state_k, uint64_t lines);
void generate_bin_file(double** matrix, uint64_t rows, uint64_t columns, const char* folder, const char* jobName, uint64_t state_k);

#endif // HEAT_SIMULATION_H