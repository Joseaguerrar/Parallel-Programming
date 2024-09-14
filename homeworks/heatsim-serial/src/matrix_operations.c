//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>

#include "matrix_operations.h"
#include <stdio.h>
#include <stdlib.h>

// Actualiza las temperaturas en la matriz de acuerdo con la fórmula de transferencia de calor
double update_temp(double **matriz, int filas, int columnas, double dtime, double alpha, double height) {
    double max_change = 0.0;
    double **new_matriz = asign_matrix(filas, columnas);

    for (int i = 1; i < filas - 1; ++i) {
        for (int j = 1; j < columnas - 1; ++j) {
            double delta = dtime * alpha / (height * height) *
                (matriz[i-1][j] + matriz[i][j+1] + matriz[i+1][j] + matriz[i][j-1] - 4 * matriz[i][j]);
            new_matriz[i][j] = matriz[i][j] + delta;

            if (abs(delta) > max_change) {
                max_change = abs(delta);
            }
        }
    }

    // Copiar las nuevas temperaturas a la matriz original
    for (int i = 1; i < filas - 1; ++i) {
        for (int j = 1; j < columnas - 1; ++j) {
            matriz[i][j] = new_matriz[i][j];
        }
    }

    free_matrix(new_matriz, filas);
    return max_change;
}

// Crea una matriz bidimensional con el número especificado de filas y columnas
double** asign_matrix(int filas, int columnas) {
    double **matriz = (double **)malloc(filas * sizeof(double *));
    for (int i = 0; i < filas; ++i) {
        matriz[i] = (double *)malloc(columnas * sizeof(double));
    }
    return matriz;
}

// Libera el espacio de memoria ocupado por una matriz bidimensional
void free_matrix(double **matriz, int filas) {
    for (int i = 0; i < filas; ++i) {
        free(matriz[i]);
    }
    free(matriz);
}
// Procesa el archivo de trabajo para realizar las simulaciones
void process_job_file(const char *job_file, const char *output_dir) {
    // Abrir el archivo de trabajo
    FILE *file = fopen(job_file, "r");
    if (!file) {
        perror("Error al abrir el archivo de trabajo");
        return;
    }

    // Preparar el nombre del archivo de reporte
    char report_file[256];
    snprintf(report_file, sizeof(report_file), "%s.tsv", job_file);
    
    if (output_dir) {
        char temp[256];
        snprintf(temp, sizeof(temp), "%s/%s", output_dir, report_file);
        strncpy(report_file, temp, sizeof(report_file));
    }

    FILE *report_fp = fopen(report_file, "w");
    if (!report_fp) {
        perror("Error al abrir el archivo de reporte");
        fclose(file);
        return;
    }

    char plate_file[64];
    double dtime, alpha, height, epsilon;
}

// Formatea el tiempo transcurrido en un formato legible
char* format_time(const time_t seconds, char* text, const size_t capacity) {
    const struct tm* gmt = gmtime(&seconds);
    snprintf(text, capacity, "%04d/%02d/%02d\t%02d:%02d:%02d", gmt->tm_year - 70,
        gmt->tm_mon, gmt->tm_mday - 1, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
    return text;
}
