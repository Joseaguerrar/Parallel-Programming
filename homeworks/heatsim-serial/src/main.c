//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <read.h>
#include <matrix_operations.h>
#include <stdio.h>
#include <stdlib.h>
#include <process_job_file.h>



/**
 * @brief Punto de entrada principal para la simulación de transferencia de calor.
 *
 * Este programa lee una matriz y los parámetros de simulación desde un archivo
 * binario, realiza iteraciones para calcular la distribución de temperatura 
 * hasta que se alcance el equilibrio térmico, y finalmente guarda los resultados
 * en un archivo binario.
 *
 * @param argc Número de argumentos pasados al programa.
 * @param argv Array de punteros a los argumentos pasados al programa. El primer
 * argumento debe ser el nombre del archivo de entrada.
 *
 * @return EXIT_SUCCESS si la simulación se completa correctamente, o EXIT_FAILURE
 * si ocurre un error (por ejemplo, si no se pasa un archivo de entrada).
 */
int main(int argc, char *argv[]) {
    /*  Verificar que se hayan pasado el nombre del archivo de trabajo
    y la ruta de salida  */
    if (argc < 3) {
        fprintf(stderr,
        "Uso: %s <archivo de trabajo> <ruta de salida>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*  Obtener el archivo de trabajo y la ruta de salida 
    desde los argumentos de línea de comandos  */ 
    const char *job_file = argv[1];
    const char *output_dir = argv[2];

    // Procesar el archivo de trabajo y ejecutar las simulaciones
    process_job_file(job_file, output_dir);

    return EXIT_SUCCESS;
}
