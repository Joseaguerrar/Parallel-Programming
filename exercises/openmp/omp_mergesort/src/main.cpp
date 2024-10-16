// Copyright 2023 Jeisson Hidalgo jeisson.hidalgo@ucr.ac.cr CC-BY-4

#include <omp.h>
#include <cassert>
#include <iostream>
#include <random>

#include "mergesort.hpp"

template <typename Type>
std::ostream& operator<<(std::ostream& out, const std::vector<Type>& values) {
  for (size_t index = 0; index < values.size(); ++index) {
    out << (index ? " " : "") << values[index];
  }
  return out;
}

template <typename Type>
bool is_sorted(const std::vector<Type>& values) {
  for (size_t index = 1; index < values.size(); ++index) {
    if (values[index] < values[index - 1]) {
      return false;
    }
  }
  return true;
}

void random_fill(const size_t count, std::vector<int>& values) {
  // See https://en.cppreference.com/w/cpp/algorithm/merge
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<> dis(0, count);

  values.reserve(count);
  for (size_t index = 0; index < count; ++index) {
    values.push_back(dis(mt));
  }
}

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
