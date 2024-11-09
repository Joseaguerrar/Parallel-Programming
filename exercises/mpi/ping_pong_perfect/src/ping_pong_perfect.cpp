//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

/**
 * @brief Punto de entrada principal del programa.
 *
 * Este programa utiliza dos procesos para simular un intercambio continuo de 
 * mensajes (ping-pong). Se verifica que haya exactamente dos procesos y se 
 * permite especificar un tiempo de espera opcional para simular un retraso 
 * en los servicios. La ejecución se detiene manualmente con Ctrl+C.
 *
 * @param argc Número de argumentos proporcionados al programa.
 * @param argv Argumentos proporcionados al programa:
 *  - argv[1] (opcional): Tiempo de espera en milisegundos entre servicios.
 * @return EXIT_SUCCESS si la simulación se ejecuta correctamente.
 * @return EXIT_FAILURE si ocurre un error.
 */
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Validación del número de procesos
    if (size != 2) {
        if (rank == 0) {
            std::cerr << "This simulation requires exactly 2 processes.\n";
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Parsear el argumento de tiempo de espera (opcional)
    int wait_time_ms = 0;
    if (argc == 2) {
        wait_time_ms = std::stoi(argv[1]);
        if (wait_time_ms < 0) {
            if (rank == 0) {
                std::cerr << "Error: Wait time must be non-negative.\n";
            }
            MPI_Finalize();
            return EXIT_FAILURE;
        }
    }

    const int TAG = 0;
    int ball = 0;  // Representa la "bola" del ping-pong
    MPI_Status status;

    if (rank == 0) {
        /**
         * @brief Lógica del proceso 0.
         *
         * El proceso 0 inicia el servicio y luego alterna entre recibir y
         * enviar la "bola" con el proceso 1.
         */
        std::cout << "0 serves\n";
        MPI_Ssend(&ball, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
        // Servicio inicial
        while (true) {
            MPI_Recv(&ball, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD, &status);
            // Recibir
            std::cout << "0 serves\n";
            std::this_thread::sleep_for
                                      (std::chrono::milliseconds(wait_time_ms));
            MPI_Ssend(&ball, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
            // Enviar de vuelta
        }
    } else if (rank == 1) {
        /**
         * @brief Lógica del proceso 1.
         *
         * El proceso 1 espera recibir la "bola" del proceso 0 y luego la
         * envía de vuelta.
         */
        while (true) {
            MPI_Recv(&ball, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
            // Recibir
            std::cout << "1 serves\n";
            std::this_thread::sleep_for
                                      (std::chrono::milliseconds(wait_time_ms));
            MPI_Ssend(&ball, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
            // Enviar de vuelta
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
