//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heat_simulation.h"

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
params_matrix* read_job_txt(const char* jobName, const char* folder,
                            uint64_t* lines) {
    FILE *jobFile;
    params_matrix *variables;
    char direction[512];
    snprintf(direction, sizeof(direction), "%s/%s", folder, jobName);

    // Contar las líneas en el archivo
    *lines = count_lines(direction);
    if (*lines <= 0) {
        fprintf(stderr, "El archivo no tiene líneas para leer\n");
        return NULL;
    }

    // Asignar memoria para las estructuras
    variables = malloc(*lines * sizeof(params_matrix));
    if (variables == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        return NULL;
    }

    // Leer los datos de cada línea del archivo .txt
    jobFile = fopen(direction, "r");
    if (jobFile == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo de trabajo %s\n",
                direction);
        free(variables);
        return NULL;
    }
    int i = 0;
    char tempFilename[256];
    while (fscanf(jobFile, "%s %lf %lf %lf %lf", tempFilename,
     &variables[i].delta_t,
     &variables[i].alpha,
     &variables[i].h,
     &variables[i].epsilon) == 5) {
        variables[i].filename = malloc(strlen(tempFilename) + 1);
        if (variables[i].filename == NULL) {
            fprintf(stderr,
                "Error al asignar memoria para filename en la línea %d\n", i);
            break;
        }
        strcpy(variables[i].filename, tempFilename);//NOLINT
        i++;
    }

    fclose(jobFile);
    return variables;
}

/**
 * @brief Cuenta el número de líneas en un archivo de texto.
 * 
 * @param fileName Nombre del archivo a leer.
 * 
 * @return El número de líneas en el archivo.
 * @return 0 si no se pudo abrir el archivo.
 */
uint64_t count_lines(const char* fileName) {
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo %s\n", fileName);
        return 0;
    }

    uint64_t lines_count = 0;
    while (!feof(file)) {
       char line_jump = fgetc(file);
       if (line_jump == '\n') {
           lines_count++;
       }
    }
    fclose(file);
    return lines_count;
}
