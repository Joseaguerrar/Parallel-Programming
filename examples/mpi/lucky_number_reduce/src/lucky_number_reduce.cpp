//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#define fail(msg) throw std::runtime_error(msg)

void generate_lucky_statistics(int process_number, int process_count);

int main(int argc, char* argv[]) {
    int error = EXIT_SUCCESS;
    if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
        try {
            int process_number = -1;
            MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

            int process_count = -1;
            MPI_Comm_size(MPI_COMM_WORLD, &process_count);

            generate_lucky_statistics(process_number, process_count);
        } catch (const std::runtime_error& exception) {
            std::cerr << exception.what() << std::endl;
            error = EXIT_FAILURE;
        }
        MPI_Finalize();
    } else {
        std::cerr << "error: could not init MPI" << std::endl;
        error = EXIT_FAILURE;
    }
    return error;
}

void generate_lucky_statistics(int process_number, int process_count) {
    // Inicializar semilla para números aleatorios
    std::srand(static_cast<unsigned int>(std::time(nullptr)) + process_number);

    // Generar número de la suerte entre 0 y 99
    const int my_lucky_number = std::rand() % 100;

    // Imprimir el número de la suerte de cada proceso
    std::cout << "Process " << process_number << ": my lucky number is "
              << my_lucky_number << std::endl;

    // Variables para los resultados globales
    int all_min = 0, all_max = 0, all_sum = 0;

    // Reducción para calcular el mínimo
    MPI_Reduce(&my_lucky_number, &all_min, 1, MPI_INT, MPI_MIN,
                                                             0, MPI_COMM_WORLD);

    // Reducción para calcular el máximo
    MPI_Reduce(&my_lucky_number, &all_max, 1, MPI_INT, MPI_MAX,
                                                             0, MPI_COMM_WORLD);

    // Reducción para calcular la suma
    MPI_Reduce(&my_lucky_number, &all_sum, 1, MPI_INT, MPI_SUM,
                                                             0, MPI_COMM_WORLD);

    // Proceso 0 calcula y muestra los resultados
    if (process_number == 0) {
        const double all_average = static_cast<double>(all_sum) / process_count;

        // Imprimir los resultados globales
        std::cout << "Process 0: all minimum: " << all_min << std::endl;
        std::cout << "Process 0: all average: " << std::fixed <<
                                                            std::setprecision(2)
                  << all_average << std::endl;
        std::cout << "Process 0: all maximum: " << all_max << std::endl;
    }
}
