//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

/**
 * @brief Programa de ejemplo usando MPI que lanza dos procesos.
 *
 * Este programa crea dos procesos. El proceso 0 imprime un mensaje,
 * mientras que el proceso 1 genera y muestra un número aleatorio entre 0 y 10.
 * El programa verifica que solo se lancen exactamente dos procesos.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Vector de argumentos de línea de comandos.
 * @return int Código de salida del programa.
 */
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  ///< Inicializa el entorno MPI

    int world_size;  ///< Número total de procesos en el comunicador global
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;  ///< Rango del proceso actual
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Verificar que solo existan 2 procesos.
    if (world_size != 2) {
        if (world_rank == 0) {
            std::cerr <<
            "Este programa debe ser ejecutado con exactamente 2 procesos."
            << std::endl;
        }
        MPI_Finalize();  ///< Finaliza el entorno MPI
        return EXIT_FAILURE;
    }

    int process_number = world_rank;
    ///< Variable que representa el número del proceso

    if (process_number == 0) {
        std::cout << "Proceso " << process_number
                             << ": Hola, soy uno de los procesos." << std::endl;
    } else if (process_number == 1) {
        std::srand(static_cast<unsigned int>(std::time(0)) + process_number);
        ///< Inicializa la semilla para el número aleatorio
        int random_number = std::rand() % 11;
        ///< Número aleatorio entre 0 y 10 (inclusive)
        std::cout << "Proceso " << process_number << ": Número generado es " <<
                                                     random_number << std::endl;
    }
    MPI_Finalize();  ///< Finaliza el entorno MPI
    return 0;
}
