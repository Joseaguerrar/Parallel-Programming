//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <mpi.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <ctime>

/**
 * @brief Computes the sum in a circular process, accumulating values across
 * all processes.
 * 
 * @param process_number The rank or ID of the current process.
 * @param process_count The total number of processes in the MPI environment.
 * @param random_number The unique random number assigned to this process.
 * @param process_hostname The hostname of the processor on which this process
 * is running.
 */
void process_sum(int process_number, int process_count, int random_number,
                 const char* process_hostname);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  // Initialize MPI environment
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    int process_number = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

    int process_count = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    // Verify that there are exactly 4 processes
    if (process_count != 4) {
      if (process_number == 0) {
        std::cerr << "error: this program requires exactly 4 processes"
                  << std::endl;
      }
      MPI_Finalize();
      return EXIT_FAILURE;
    }

    // Get the hostname of the current process
    char process_hostname[MPI_MAX_PROCESSOR_NAME];
    int hostname_length = -1;
    MPI_Get_processor_name(process_hostname, &hostname_length);

    // Seed random generator and assign a random number for each process
    std::srand(time(nullptr) + process_number);  // Different seed per process
    int random_number = std::rand() % 101;

    try {
      process_sum(process_number, process_count, random_number,
                  process_hostname);
    } catch (const std::runtime_error& exception) {
      std::cerr << "error: " << exception.what() << std::endl;
      error = EXIT_FAILURE;
    }

    // Finalize MPI environment
    MPI_Finalize();
  } else {
    std::cerr << "error: could not init MPI" << std::endl;
    error = EXIT_FAILURE;
  }
  return error;
}

/**
 * @brief Performs a circular sum operation across all processes. Each process
 * adds its unique random number to an accumulated sum that circulates among
 * the processes. This operation repeats for a fixed number of cycles.
 *
 * @param process_number The rank of the current process in the MPI communicator.
 * @param process_count Total number of processes in the MPI communicator.
 * @param random_number A random number generated for the current process.
 * @param process_hostname The hostname of the machine where this process is
 * executed.
 */
void process_sum(int process_number, int process_count, int random_number,
                 const char* process_hostname) {
  int previous_process = (process_count + process_number - 1) % process_count;
  int next_process = (process_number + 1) % process_count;

  int accumulated_sum = 0;  // Initial sum to start with

  for (int i = 0; i < 4; ++i) {
    if (i == 0) {
      // Start the accumulation with each process's random number
      accumulated_sum = random_number;
    } else {
      // Receive the accumulated sum from the previous process
      if (MPI_Recv(&accumulated_sum, 1, MPI_INT, previous_process, 0,
                   MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
        std::cout << "error: could not receive sum from previous process"
                  << std::endl;
      }
      // Add the process's random number to the accumulated sum
      accumulated_sum += random_number;
    }

    // Send the new accumulated sum to the next process
    if (MPI_Send(&accumulated_sum, 1, MPI_INT, next_process, 0, MPI_COMM_WORLD)
        != MPI_SUCCESS) {
      std::cout << "error: could not send sum to next process" << std::endl;
    }
  }

  // Final print statement after all cycles are complete
  std::cout << "Process " << process_number << " of " << process_count
            << " in " << process_hostname
            << ", my random number is " << random_number
            << ", and the final accumulated sum is " << accumulated_sum
            << std::endl;
}
