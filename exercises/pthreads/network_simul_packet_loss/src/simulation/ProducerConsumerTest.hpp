/// @copyright 2020 ECCI, Universidad de Costa Rica.
/// All rights reserved. This code is released under the GNU Public License version 3.
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#ifndef PRODUCERCONSUMERTEST_HPP
#define PRODUCERCONSUMERTEST_HPP

#include <cstddef>
#include <vector>

#include "common.hpp"

// Forward declarations
class AssemblerTest;
class ConsumerTest;
class DispatcherTest;
class ProducerTest;

/**
 * @brief Controller class that simulates the message passing between
 * producer and consumers through a dispatcher.
 * 
 * This class orchestrates the creation of the producer, dispatcher, consumers, 
 * and assembler objects. It sets up communication between them, starts the 
 * threads for each component, and handles the simulation of packet loss.
 */
class ProducerConsumerTest {
  DISABLE_COPY(ProducerConsumerTest);  
  ///< Macro to disable copy constructor and assignment.

 private:
  /// Number of packages to be produced.
  size_t packageCount = 0;
  
  /// Number of consumer threads.
  size_t consumerCount = 0;
  
  /// Delay of producer to create a package, negative for max random delay.
  int productorDelay = 0;
  
  /// Delay of dispatcher to dispatch a package, negative for max random delay.
  int dispatcherDelay = 0;
  
  /// Delay of consumer to consume a package, negative for max random delay.
  int consumerDelay = 0;
  
  /// Percentage of packet loss during transmission.
  double package_loss_percent = 0.0;

 private:
  /// Producer of the simulated network messages.
  ProducerTest* producer = nullptr;
  
  /// Dispatcher that sends the network messages to consumers.
  DispatcherTest* dispatcher = nullptr;
  
  /// Consumers that receive and process the network messages.
  std::vector<ConsumerTest*> consumers;
  
  /// Assembler to simulate packet loss in the network.
  AssemblerTest* assembler = nullptr;

 public:
  /**
   * @brief Default constructor.
   */
  ProducerConsumerTest() = default;

  /**
   * @brief Destructor. Cleans up the dynamically allocated resources.
   */
  ~ProducerConsumerTest();

  /**
   * @brief Starts the producer-consumer simulation.
   * 
   * This method initializes the simulation components, starts their threads,
   * and manages their interaction throughout the simulation.
   * 
   * @param argc Number of command-line arguments.
   * @param argv Array of command-line arguments.
   * @return int Returns EXIT_SUCCESS if simulation runs successfully, otherwise EXIT_FAILURE.
   */
  int start(int argc, char* argv[]);

 private:
  /**
   * @brief Analyzes the command-line arguments provided to the program.
   * 
   * This method validates and stores the values provided for the number of packages,
   * the number of consumers, and the delays for each component, as well as the 
   * packet loss percentage.
   * 
   * @param argc Number of command-line arguments.
   * @param argv Array of command-line arguments.
   * @return int Returns EXIT_SUCCESS if arguments are valid, or EXIT_FAILURE if there's an error.
   */
  int analyzeArguments(int argc, char* argv[]);
};

#endif  // PRODUCERCONSUMERTEST_HPP
