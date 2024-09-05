//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <simulation_reader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Implementación de la función que lee el archivo de texto y extrae los parámetros de simulación.
 * 
 * Esta función abre un archivo de texto que contiene múltiples líneas, cada una describiendo una simulación.
 * Extrae el nombre del archivo de la placa, el tiempo de cada etapa, la difusividad térmica, el tamaño de las celdas
 * y el criterio de convergencia epsilon, y almacena estos valores en un array de estructuras `SimulationParams`.
 * 
 * @param filename Nombre del archivo de texto que contiene los parámetros de las simulaciones.
 * @param params Puntero a un array de estructuras `SimulationParams` donde se almacenarán los parámetros leídos.
 * @param max_params Número máximo de simulaciones que puede almacenar el array `params`.
 * 
 * @return int Número de simulaciones leídas desde el archivo. Devuelve EXIT_FAILURE si hay un error al abrir el archivo.
 */
int read_simulation_file(const char *filename,
                         SimulationParams *params, int max_params) {
    // Abrir el archivo de texto en modo lectura
    FILE *file = fopen(filename, "r");
    if (!file) {
        // Imprimir un mensaje de error si no se puede abrir el archivo
        perror("Error al abrir el archivo de simulación");
        return EXIT_FAILURE;
    }

    // Buffer para almacenar cada línea del archivo de texto
    char line[MAX_LINE_LENGTH];
    int count = 0;  // Contador para el número de simulaciones leídas

    /* Leer cada línea del archivo mientras no se alcance el final
    del archivo o el límite de simulaciones*/
    while (fgets(line, sizeof(line), file) && count < max_params) {
        // Analizar la línea leída y extraer los parámetros de simulación
        int items_read = sscanf(line, "%s %lf %lf %lf %lf",
                                // Nombre del archivo de la placa
                                params[count].plate_file,
                                // Tiempo de cada etapa (Δt)
                                &params[count].dtime,
                                // Difusividad térmica (α)
                                &params[count].alpha,
                                // Tamaño de las celdas (h)
                                &params[count].height,
                                // Criterio de convergencia (ε)
                                &params[count].epsilon);
        /*Si epsilon no se leyó (sólo se leyeron 4 valores),
        usar un valor predeterminado*/
        if (items_read < 5) {
            params[count].epsilon = DEFAULT_EPSILON;
        }
        // Incrementar el contador de simulaciones leídas
        count++;
    }
    fclose(file);
    return count;
}
