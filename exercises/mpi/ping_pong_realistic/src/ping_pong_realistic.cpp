//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <iostream>
#include <random>
#include <cstdlib>

/**
 * @brief Simula un servicio con base en la probabilidad de acierto.
 *
 * Genera un número aleatorio entre 0 y 100 y verifica si está dentro del rango
 * de éxito definido por `accuracy`.
 *
 * @param accuracy Probabilidad de acierto (entre 0 y 100).
 * @return true si el servicio es exitoso, false en caso contrario.
 */
bool simulate_service(double accuracy) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen) <= accuracy;  // Devuelve true si el servicio es exitoso
}

/**
 * @brief Punto de entrada principal del programa.
 *
 * Este programa simula un juego de ping-pong entre dos procesos utilizando MPI.
 * Los jugadores alternan turnos para realizar servicios. Si un jugador falla
 * su servicio, el oponente gana la ronda. El juego continúa hasta que uno de
 * los jugadores alcanza el puntaje necesario para ganar.
 *
 * @param argc Número de argumentos proporcionados al programa.
 * @param argv Argumentos proporcionados al programa:
 * - `argv[1]` Número de puntos necesarios para ganar.
 * - `argv[2]` Probabilidad de acierto del jugador 1 (proceso 0).
 * - `argv[3]` Probabilidad de acierto del jugador 2 (proceso 1).
 * @return EXIT_SUCCESS si el programa se ejecuta correctamente.
 * @return EXIT_FAILURE si ocurre un error de validación.
 */
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    ///< Identificador del proceso actual.
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    ///< Número total de procesos.

    // Validación de argumentos
    if (size != 2 || argc != 4) {
        if (rank == 0) {
            std::cerr << "Usage: mpiexec -n 2 " << argv[0]
                      << " <win_score> <player1_accuracy> <player2_accuracy>\n";
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int win_score = std::stoi(argv[1]);
    ///< Puntos necesarios para ganar.
    double player1_accuracy = std::stod(argv[2]);
    ///< Probabilidad de acierto del jugador 1.
    double player2_accuracy = std::stod(argv[3]);
    ///< Probabilidad de acierto del jugador 2.

    int scoreboard[2] = {0, 0};  ///< Marcador de los jugadores (proceso 0 y 1).
    bool game_over = false;   ///< Indica si el juego ha terminado.
    int services = 0;   ///< Contador de servicios realizados.
    int rounds = 0;   ///< Contador de rondas jugadas.

    while (!game_over) {
        bool scored = false;
        ///< Resultado del servicio actual (true = exitoso, false = fallo).
        int round_winner = -1;
        ///< Ganador de la ronda actual (-1 = nadie).

        if (rank == services % 2) {  // Jugador en turno
            double success_rate = (rank == 0) ? player1_accuracy :
                                                               player2_accuracy;
            scored = simulate_service(success_rate);

            if (!scored) {
                // Si falla, el oponente gana la ronda
                scoreboard[1 - rank]++;
                round_winner = 1 - rank;
            }

            MPI_Send(&scored, 1, MPI_C_BOOL, 1 - rank, 0, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&scored, 1, MPI_C_BOOL, 1 - rank, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            if (!scored) {
                // Si el oponente falla, el jugador en turno gana la ronda
                scoreboard[rank]++;
                round_winner = rank;
            }
        }

        services++;  // Incrementa el número de servicios

        // Solo el proceso 0 imprime los resultados de cada ronda
        if (rank == 0 && round_winner != -1) {
            std::cout << rounds + 1 << ": " << services - 1 << " "
                      << round_winner << std::endl;
            rounds++;
        }

        // Verificar si el juego ha terminado
        if (rank == 0 &&
            (scoreboard[0] >= win_score || scoreboard[1] >= win_score)) {
            game_over = true;
            int final_winner = (scoreboard[0] >= win_score) ? 0 : 1;
            std::cout << "Process " << final_winner << " wins " << scoreboard[0]
                      << " to " << scoreboard[1] << std::endl;
        }

        // Sincronizar el estado del juego y marcador
        MPI_Bcast(&game_over, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
        MPI_Bcast(scoreboard, 2, MPI_INT, 0, MPI_COMM_WORLD);

        // Reiniciar el ganador de la ronda para la siguiente iteración
        round_winner = -1;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
