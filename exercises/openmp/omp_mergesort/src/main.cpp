// Copyright 2023 Jeisson Hidalgo jeisson.hidalgo@ucr.ac.cr CC-BY-4

#include <omp.h>
#include <cassert>
#include <iostream>
#include <random>

#include "mergesort.hpp"

/**
 * @brief Sobrecarga del operador de salida para vectores.
 * 
 * Esta función imprime los elementos de un vector en un formato amigable.
 * 
 * @tparam Type El tipo de datos almacenado en el vector.
 * @param out Flujo de salida donde se escriben los elementos del vector.
 * @param values El vector cuyos elementos serán impresos.
 * @return std::ostream& Referencia al flujo de salida.
 */
template <typename Type>
std::ostream& operator<<(std::ostream& out, const std::vector<Type>& values) {
  for (size_t index = 0; index < values.size(); ++index) {
    out << (index ? " " : "") << values[index];
  }
  return out;
}

/**
 * @brief Verifica si un vector está ordenado.
 * 
 * Esta función recorre el vector y comprueba si sus elementos están en orden no decreciente.
 * 
 * @tparam Type El tipo de datos almacenado en el vector.
 * @param values El vector a verificar.
 * @return true Si el vector está ordenado.
 * @return false Si el vector no está ordenado.
 */
template <typename Type>
bool is_sorted(const std::vector<Type>& values) {
  for (size_t index = 1; index < values.size(); ++index) {
    if (values[index] < values[index - 1]) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Llena un vector con números aleatorios.
 * 
 * Esta función genera números aleatorios en el rango [0, count] y los almacena en el vector `values`.
 * 
 * @param count La cantidad de números aleatorios a generar.
 * @param values Referencia al vector donde se almacenarán los números aleatorios.
 */
void random_fill(const size_t count, std::vector<int>& values) {
  // Ver https://en.cppreference.com/w/cpp/algorithm/merge
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, count);

  values.reserve(count);
  for (size_t index = 0; index < count; ++index) {
    values.push_back(dis(mt));
  }
}

/**
 * @brief Función principal del programa.
 * 
 * Esta función controla el flujo principal del programa, incluyendo la lectura de datos, el
 * proceso de ordenamiento mediante mergesort y la verificación del orden del resultado.
 * 
 * @param argc Cantidad de argumentos pasados por línea de comandos.
 * @param argv Arreglo de punteros a los argumentos pasados por línea de comandos.
 * @return int Código de salida del programa.
 */
int main(int argc, char* argv[]) {
  // Analizar argumentos
  const size_t count = argc >= 2 ? std::stoull(argv[1]) : 0;
  const int thread_count = argc >= 3 ? atoi(argv[2]) : omp_get_max_threads();
  const bool verbose = argc >= 4 ? std::stoi(argv[3]) : 1;

  // Llenar el vector de valores
  std::vector<int> values;
  if (count == 0) {
    // No hay cantidad de elementos aleatorios, leer desde stdin
    int value = 0;
    while (std::cin >> value) {
      values.push_back(value);
    }
  } else {
    // Llenar el vector con valores aleatorios
    random_fill(count, values);
    // Si el modo verbose está activado, imprimir el vector aleatorio
    if (verbose == 1) {
      std::cout << values << std::endl;
    }
  }

  // Configurar la cantidad de hilos que OpenMP va a usar
  omp_set_num_threads(thread_count);

  // Ordenar el vector usando el mergesort paralelizado
  mergesort(values);

  // Imprimir los valores ordenados
  if (verbose) {
    std::cout << values << std::endl;
  }

  // Asegurarse de que el vector esté ordenado
  assert(is_sorted(values));
}
