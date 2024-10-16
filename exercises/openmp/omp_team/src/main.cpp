// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <iostream>
#include <omp.h> //NOLINT
#include <cstdlib>

/**
 * @brief Función principal del programa.
 * 
 * Esta función determina el número de hilos a utilizar, establece la cantidad de hilos y luego imprime 
 * un mensaje desde cada hilo en una sección crítica para evitar la mezcla de la salida.
 * 
 * @param argc Número de argumentos pasados por línea de comandos.
 * @param argv Array de punteros a los argumentos pasados por línea de comandos.
 * @return int Código de salida del programa.
 */
int main(int argc, char* argv[]) {
    // Determinar el número de hilos
    const int thread_count = (argc == 2) ? std::atoi(argv[1]) :
                                                            omp_get_num_procs();

    // Establecer el número de hilos
    omp_set_num_threads(thread_count);

    // Crear los hilos en paralelo
    #pragma omp parallel default(none) shared(thread_count, std::cout)
    {
        // Cada hilo obtiene su número de identificación
        int thread_id = omp_get_thread_num();

        // Usamos una sección crítica para evitar la mezcla en la salida
        #pragma omp critical
        {
            std::cout << "Hello from secondary thread " << thread_id
                      << " of " << thread_count << std::endl;
        }
    }

    return 0;
}
