//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
/**
 * @file mpi_lucky_statistics.cpp
 * @brief Example program using MPI to compute distributed statistics.
 * 
 * This program demonstrates how to use MPI to calculate the minimum, 
 * maximum, and average of randomly generated "lucky numbers" 
 * across multiple processes.
 */

#include <mpi.h>
#include <iostream>
#include <stdexcept>
#include "UniformRandom.hpp"

#define fail(msg) throw std::runtime_error(msg)

/**
 * @brief Computes and outputs distributed statistics of "lucky numbers".
 * 
 * Each process generates a random "lucky number" and calculates the minimum,
 * maximum, and average across all processes using MPI_Allreduce.
 * 
 * @param process_number Rank of the calling process.
 * @param process_count Total number of processes in the MPI communicator.
 */
void generate_lucky_statistics(int process_number, int process_count);

/**
 * @brief Entry point of the program.
 * 
 * Initializes MPI, determines process-specific information, and invokes 
 * the function to generate distributed statistics.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Exit status code (EXIT_SUCCESS or EXIT_FAILURE).
 */
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    try {
      int process_number = -1;
      MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

      int process_count = -1;
      MPI_Comm_size(MPI_COMM_WORLD, &process_count);

      char process_hostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
      int hostname_length = -1;
      MPI_Get_processor_name(process_hostname, &hostname_length);

      generate_lucky_statistics(process_number, process_count);
    } catch (const std::runtime_error& exception) {
      std::cout << exception.what() << std::endl;
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
  /**
   * @brief Generates and compares "lucky numbers" across MPI processes.
   * 
   * Each process generates a random lucky number and participates in
   * distributed reduction operations to calculate the minimum, maximum, and
   * average lucky numbers across all processes. It outputs results specific
   * to its own lucky number.
   */

  // Generate my lucky number
  UniformRandom<int> uniformRandom(process_number);
  const int my_lucky_number = uniformRandom.between(0, 100);

  std::cout << "Process " << process_number << ": my lucky number is "
    << my_lucky_number << std::endl;

  int all_min = -1;
  int all_max = -1;
  int all_sum = -1;

  // Update distributed statistics from processes' lucky numbers
  if (MPI_Allreduce(/*input*/ &my_lucky_number, /*output*/ &all_min, /*count*/ 1
                           , MPI_INT, MPI_MIN, MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could not reduce min");
  }
  if (MPI_Allreduce(/*input*/ &my_lucky_number, /*output*/ &all_max, /*count*/ 1
                           , MPI_INT, MPI_MAX, MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could not reduce max");
  }
  if (MPI_Allreduce(/*input*/ &my_lucky_number, /*output*/ &all_sum, /*count*/ 1
                           , MPI_INT, MPI_SUM, MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could not reduce sum");
  }

  const double all_average = static_cast<double>(all_sum) / process_count;
  if (my_lucky_number == all_min) {
    std::cout << "Process " << process_number << ": my lucky number ("
      << my_lucky_number << ") is the minimum (" << all_min << ")" << std::endl;
  }

  if (my_lucky_number < all_average) {
    std::cout << "Process " << process_number << ": my lucky number ("
      << my_lucky_number << ") is less than the average (" << all_average
      << ")" << std::endl;
  } else if (my_lucky_number > all_average) {
    std::cout << "Process " << process_number << ": my lucky number ("
      << my_lucky_number << ") is greater than the average (" << all_average
      << ")" << std::endl;
  } else {
    std::cout << "Process " << process_number << ": my lucky number ("
      << my_lucky_number << ") is equal to the average (" << all_average
      << ")" << std::endl;
  }

  if (my_lucky_number == all_max) {
    std::cout << "Process " << process_number << ": my lucky number ("
      << my_lucky_number << ") is the maximum (" << all_max << ")" << std::endl;
  }
}
