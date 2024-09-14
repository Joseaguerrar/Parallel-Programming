//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include "matrix_operations.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo de trabajo> [directorio de salida]\n", argv[0]);
        return 1;
    }

    const char *job_file = argv[1];
    const char *output_dir = (argc > 2) ? argv[2] : NULL;

    // Luego ejecutar la simulación si todo está bien
    process_job_file(job_file, output_dir);

    return 0;
}

