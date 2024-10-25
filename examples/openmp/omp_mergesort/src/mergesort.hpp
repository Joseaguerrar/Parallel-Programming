// Copyright 2023 Jeisson Hidalgo jeisson.hidalgo@ucr.ac.cr CC-BY-4
#include <algorithm>
#include <vector>
#include <cstddef>
#include <omp.h> //NOLINT

/**
 * @brief Función auxiliar que realiza el algoritmo de mergesort en paralelo.
 * 
 * Esta función se encarga de dividir recursivamente el vector en dos mitades, ordenarlas por separado
 * y luego fusionarlas. Se paraleliza usando OpenMP mediante la creación de tareas para el ordenamiento
 * de las mitades izquierda y derecha.
 * 
 * @tparam Type Tipo de los elementos almacenados en el vector.
 * @param values Referencia al vector que contiene los elementos a ordenar.
 * @param left Índice del límite izquierdo del subvector a ordenar.
 * @param right Índice del límite derecho del subvector a ordenar.
 */
template <typename Type>
void mergesort(std::vector<Type>& values, const ptrdiff_t left,
    const ptrdiff_t right) {
  // Número de elementos a ordenar
  const ptrdiff_t count = right - left;
  if (count > 0) {
    const size_t mid = (left + right) / 2;

    #pragma omp task shared(values)  // Crea una tarea paralela
    // Ordenar la mitad izquierda del vector
    mergesort(values, left, mid);

    #pragma omp task shared(values)  // Crea una tarea paralela
    // Ordenar la mitad derecha del vector
    mergesort(values, mid + 1, right);

    #pragma omp taskwait  // Espera a que ambas tareas terminen

    // Ambas mitades están ordenadas, fusionarlas en un vector temporal
    std::vector<Type> temp;
    temp.reserve(count + 1);
    std::merge(values.begin() + left, values.begin() + mid + 1,
        values.begin() + mid + 1, values.begin() + right + 1,
        std::back_inserter(temp));

    // Copiar el vector temporal ordenado de vuelta al vector original
    std::copy(temp.begin(), temp.end(), values.begin() + left);
  }
}

/**
 * @brief Función principal de mergesort paralelizado.
 * 
 * Esta función inicia una región paralela de OpenMP y llama a la función auxiliar `mergesort`
 * para comenzar el proceso de ordenamiento del vector completo.
 * 
 * @tparam Type Tipo de los elementos almacenados en el vector.
 * @param values Referencia al vector que contiene los elementos a ordenar.
 */
template <typename Type>
void mergesort(std::vector<Type>& values) {
  // Iniciar la región paralela
  #pragma omp parallel
  {
    #pragma omp single  // Solo un hilo ejecuta el mergesort inicial
    mergesort(values, 0, values.size() - 1);
  }
}
