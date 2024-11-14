// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <mpi.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define fail(msg) throw std::runtime_error(msg)

void simulate_relay_race(int argc, char* argv[], int process_number
  , int process_count);
void run_stage1(int stage1_delay, int process_number, int team_count);
void run_stage2(int stage2_delay, int process_number, int team_count);
void referee(int team_count);

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

      simulate_relay_race(argc, argv, process_number, process_count);
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

void simulate_relay_race(int argc, char* argv[], int process_number
  , int process_count) {
  if (argc == 3) {
    if (process_count >= 3 && process_count % 2 == 1) {
      const int team_count = (process_count - 1) / 2;
      const int stage1_delay = atoi(argv[1]);
      const int stage2_delay = atoi(argv[2]);

      if (process_number == 0) {
        referee(team_count);
      } else if (process_number <= team_count) {
        run_stage1(stage1_delay, process_number, team_count);
      } else {
        run_stage2(stage2_delay, process_number, team_count);
      }
    } else {
      fail("error: process count must be odd and greater than 3");
    }
  } else {
    fail("usage: relay_race_dist stage1_delay stage2_delay");
  }
}

void run_stage1(int stage1_delay, int process_number, int team_count) {
  // wait_barrier()
  if (MPI_Barrier(MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could wait for barrier");
  }
  usleep(1000 * stage1_delay);
  const int peer = process_number + team_count;
  bool baton = true;
  // send(&baton, 1, peer)
  if (MPI_Send(&baton, /*count*/ 1, MPI_C_BOOL, peer, /*tag*/ 0
    , MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could not send baton");
  }
}

void run_stage2(int stage2_delay, int process_number, int team_count) {
  // wait_barrier()
  if (MPI_Barrier(MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could wait for barrier");
  }
  int peer = process_number - team_count;
  bool baton = false;
  // receive(&baton, 1, peer)
  if (MPI_Recv(&baton, /*capacity*/ 1, MPI_C_BOOL, /*source*/ peer
    , /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS ) {
    fail("could not receive baton");
  }
  usleep(1000 * stage2_delay);
  // send(&peer, 1, 0)
  if (MPI_Send(&peer, /*count*/ 1, MPI_INT, /*target*/ 0, /*tag*/ 0
    , MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could not send team number to referee");
  }
}

void referee(int team_count) {
  const double start_time = MPI_Wtime();
  // wait_barrier()
  if (MPI_Barrier(MPI_COMM_WORLD) != MPI_SUCCESS) {
    fail("error: could wait for barrier");
  }
  int place = 0;
  for (int index = 0; index < team_count; ++index) {
    int team = 0;
    // receive(&team, 1, any_process)
    if (MPI_Recv(&team, /*capacity*/ 1, MPI_INT, MPI_ANY_SOURCE
      , /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS ) {
      fail("error: could not receive team number");
    }
    const double elapsed = MPI_Wtime() - start_time;
    ++place;
    std::cout << "Place " << place << ": team " << team << " in " << elapsed
      << "s" << std::endl;
  }
}