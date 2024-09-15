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
    
    int i = 0;
    char tempFilename[256];
    while (fscanf(jobFile, "%s %lf %lf %lf %lf", tempFilename,
     &variables_formulas[i].delta_t,
      &variables_formulas[i].alpha,
       &variables_formulas[i].h,
        &variables_formulas[i].epsilon) == 5)
    {
        variables_formulas[i].filename = malloc(strlen(tempFilename) + 1);
        if (variables_formulas[i].filename == NULL) {
            fprintf(stderr, "Error al asignar memoria para filename en la línea %d\n", i);
            break;
        }
        strcpy(variables_formulas[i].filename, tempFilename);
        i++;
    }

    fclose(jobFile);
    return variables_formulas;
    
}


uint64_t count_lines(const char* fileName){
    FILE *file;
    file=fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo %s\n", fileName);
        return 0;
    }

    uint64_t lines_count=0;
    while (!feof(file))
    {
       char line_jump=fgetc(file);
       if (line_jump=='\n')
       {
        lines_count++;
       }
    }
    fclose(file);
    return lines_count;
    
}
