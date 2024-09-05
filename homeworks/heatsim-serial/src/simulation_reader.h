//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef SIMULATION_READER_H
#define SIMULATION_READER_H

#define MAX_LINE_LENGTH 256
#define DEFAULT_EPSILON 0.0

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

#endif  // SIMULATION_READER_H
