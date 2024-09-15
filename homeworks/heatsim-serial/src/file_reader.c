//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heat_simulation.h"
// Lee el archivo de texto y extrae los parámetros de simulación para cada placa
// Implementación de leer_job_txt
formula_p* read_job_txt(const char* jobName, const char* folder, uint64_t* lines) {
    FILE *jobFile;
    formula_p *variables_formulas;
    char direction[512];
    snprintf(direction, sizeof(direction), "%s/%s", folder, jobName);

    //Contar las líneas en el archivo
    *lines=count_lines(direction);
    if (*lines<=0)
    {
        fprintf(stderr, " El archivo no tiene lineas para leer\n");
        return NULL;
    }

    //Asignar memoria para las estructuras
    variables_formulas= malloc(*lines * sizeof(formula_p));
    if (variables_formulas==NULL)
    {
        fprintf(stderr, "Error al asignar memoria\n");
        return NULL;
    }

    //Leer los datos de cada línea del archivo .txt
    jobFile=fopen(direction, "r");
    if (jobFile==NULL)
    {
        fprintf(stderr, "No se pudo abrir el archivo de trabajo %s\n", direction);
        free(variables_formulas);
        return NULL;
    }
    
    
}
