//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#define fail(msg) throw std::runtime_error(msg)

/**
 * @brief Calcula estadísticas de los números de la suerte generados por los procesos.
 *
 * Esta función genera un número de la suerte para el proceso actual, utiliza
 * reducciones de MPI para calcular el mínimo, máximo y promedio de los números
 * generados por todos los procesos, y los imprime en el proceso 0.
 *
 * @param process_number Número del proceso actual.
 * @param process_count Total de procesos ejecutándose en MPI.
 */
void generate_lucky_statistics(int process_number, int process_count);

/**
 * @brief Punto de entrada del programa.
 *
 * Inicializa MPI, obtiene el número de proceso y el total de procesos,
 * y llama a la función para generar estadísticas de números de la suerte.
 *
 * @param argc Cantidad de argumentos de la línea de comandos.
 * @param argv Arreglo de argumentos de la línea de comandos.
 * @return EXIT_SUCCESS si la ejecución es exitosa, EXIT_FAILURE en caso de error.
 */
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
                  std::setprecision(2) << all_average << std::endl;
        std::cout << "Process 0: all maximum: " << all_max << std::endl;
    }
}
