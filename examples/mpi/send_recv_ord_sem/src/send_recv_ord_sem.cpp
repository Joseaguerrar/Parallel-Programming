// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <mpi.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// DistributedExeption(process_number, exception_code, msg)
#define fail(msg) throw std::runtime_error(msg)

void greet(int process_number, int process_count, const char* process_hostname);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    int process_number = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

    int process_count = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    char process_hostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
    int hostname_length = -1;
    MPI_Get_processor_name(process_hostname, &hostname_length);

    try {
      greet(process_number, process_count, process_hostname);
    } catch (const std::runtime_error& exception) {
      std::cerr << "error: " << exception.what() << std::endl;
      error = EXIT_FAILURE;
    }

    MPI_Finalize();
  } else {
    std::cerr << "error: could not init MPI" << std::endl;
    error = EXIT_FAILURE;
  }
  return error;
}

void greet(int process_number, int process_count
    , const char* process_hostname) {
  const int previous_process = (process_count + process_number - 1)
    % process_count;
  const int next_process = (process_number + 1) % process_count;
  bool can_print = true;

  if (process_number > 0) {
    // receive(&can_print, 1, previous_process)
    if (MPI_Recv(&can_print, /*count*/ 1, MPI_C_BOOL, previous_process
      , /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS ) {
      fail("could not receive message");
    }
  }

  std::cout << "Hello from main thread of process " << process_number
    << " of " << process_count << " on " << process_hostname << std::endl;

  // send(can_print, 1, next_process);
  if (MPI_Send(&can_print, /*count*/ 1, MPI_C_BOOL, next_process, /*tag*/ 0
    , MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("could not send message");
  }
}