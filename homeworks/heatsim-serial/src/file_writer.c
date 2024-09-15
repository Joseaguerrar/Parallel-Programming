//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heat_simulation.h"

//Implementación de generate_report_file
void generate_report_file(const char* folder, const char* jobName, params_matrix* variables, uint64_t* states_k, uint64_t lines){
    char report_name[1024]; char jobName_no_txt[512]; char formatted_time[48];


    strncpy(jobName_no_txt, jobName, sizeof(jobName_no_txt)-1);
    jobName_no_txt[sizeof(jobName_no_txt)-1]= '\0'; 
    char* position =strstr(jobName_no_txt,".txt");
    if (position)
    {
        *position ='\0';
    }
    
    snprintf(report_name, sizeof(report_name), "%s/%s.tsv", folder, jobName_no_txt);
    FILE* report_file = fopen(report_name, "w");
    if (report_file == NULL) {
        fprintf(stderr, "No se pudo crear el archivo de reporte %s\n", report_name);
        return;
    }

    for (uint64_t i = 0; i < lines; i++) {
        time_t tiempo_transcurrido = states_k[i] * variables[i].delta_t;
        format_time(tiempo_transcurrido, formatted_time, sizeof(formatted_time));
        fprintf(report_file, "%s\t%lf\t%lf\t%lf\t%lg\t%lu\t%s\n",
                variables[i].filename,
                variables[i].delta_t,
                variables[i].alpha,
                variables[i].h,
                variables[i].epsilon,
                states_k[i],
                formatted_time);
    }

    fclose(report_file);
}

// Implementación de generar_archivo_bin
void generate_bin_file(double** matrix, uint64_t rows, uint64_t columns, const char* folder, const char* jobName, uint64_t states_k) {
    char file_name[1024]; char base_name[512];
    strncpy(base_name, jobName, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';
    char* pos = strstr(base_name, ".bin");
    if (pos) {
        *pos = '\0';
    }

    snprintf(file_name, sizeof(file_name), "%s/%s-%lu.bin", folder, base_name, states_k);
    FILE* output_file = fopen(file_name, "wb");
    if (output_file == NULL) {
        fprintf(stderr, "No se pudo crear el archivo binario %s\n", file_name);
        return;
    }

    fwrite(&rows, sizeof(uint64_t), 1, output_file);
    fwrite(&columns, sizeof(uint64_t), 1, output_file);
    for (uint64_t i = 0; i < rows; i++) {
        fwrite(matrix[i], sizeof(double), columns, output_file);
    }
    fclose(output_file);
}