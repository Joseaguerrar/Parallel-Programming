// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0
#include <iostream>

int main() {
  #pragma omp parallel
  {
    #pragma omp critical
    std::cout << "Hello from secondary thread" << std::endl;
  }
}