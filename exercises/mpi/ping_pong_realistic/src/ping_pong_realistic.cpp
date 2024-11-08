#include <iostream>
#include <mpi.h>
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Error: El programa requiere exactamente 4 argumentos." << std::endl;
        return 1;
    }
    int requiered_wins = std::stoi(argv[1]);
    int prob_p1 = std::stoi(argv[2]);
    int prob_p2 = std::stoi(argv[3]);
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    while (true)
    {
        int generated_number = generate_random_number(rank, size, prob_p1, prob_p2);
        if (generated_number == invalid(miss))
        {
            std::cout << "Jugador " << rank << " perdio el juego." << std::endl;
            rank_wins = rank_wins + 1;
            MPI_Finalize();
            return 0;
        }
        else
        {
            send_message(rank, size, generated_number);
        }
        if (rank_wins == requiered_wins)
        {
            std::cout << "Jugador " << rank << " gano la partida." << std::endl;
            std::cout << "global score " << score << std::endl;
            MPI_Finalize();
            return 0;
        }
    }
}
int generate_random_number(int rank, int size, int prob_p1, int prob_p2)
{
    int generated_number = rand() % 100;
    if (rank == 0)
    {
        if (generated_number < prob_p1)
        {
            return 1;
        }
        else
        {
            return invalid(miss);
        }
    }
    else
    {
        if (generated_number < prob_p2)
        {
            return 2;
        }
        else
        {
            return invalid(miss);
        }
    }
}