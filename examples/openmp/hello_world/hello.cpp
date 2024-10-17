// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0
#include <iostream>

/**
 * @brief Función principal que ejecuta el programa.
 * 
 * Esta función crea múltiples hilos usando OpenMP y cada hilo imprime un mensaje. 
 * La sección crítica de OpenMP garantiza que la salida de cada hilo no se mezcle.
 * 
 * @return int Código de salida del programa.
 */
int main() {
  #pragma omp parallel
  {
    #pragma omp critical
    std::cout << "Hello from secondary thread" << std::endl;
  }
}
