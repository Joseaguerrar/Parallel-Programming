#include <mpi.h>
#include <iostream>
#include <stdexcept>

#include "UniformRandom.hpp"

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
  // TODO(you): Generate my lucky number
  const int my_lucky_number = -1;

  int all_min = -1;
  int all_max = -1;
  int all_sum = -1;

  // TODO(you): Update distributed statistics from processes' lucky numbers

  const double all_average = double(all_sum) / process_count;
  std::cout << "Process " << process_number << ": all minimum = "
    << all_min << std::endl;
  std::cout << "Process " << process_number << ": all average = "
    << all_average << std::endl;
  std::cout << "Process " << process_number << ": all maximum = "
    << all_max << std::endl;

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