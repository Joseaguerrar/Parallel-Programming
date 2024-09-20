/// @copyright 2020 ECCI, Universidad de Costa Rica.
/// All rights reserved. This code is released
/// under the GNU Public License version 3.
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#include <cstdlib>
#include <iostream>

#include "ProducerConsumerTest.hpp"
#include "ConsumerTest.hpp"
#include "DispatcherTest.hpp"
#include "ProducerTest.hpp"
#include "AssemblerTest.hpp"

/// Usage message for the program, detailing expected arguments.
const char* const usage =
  "Usage: prodcons packages consumers prod_delay disp_delay cons_delay\n"
  "\n"
  "  packages    number of packages to be produced\n"
  "  consumers   number of consumer threads\n"
  "  prod_delay  delay of producer to create a package\n"
  "  disp_delay  delay of dispatcher to dispatch a package\n"
  "  cons_delay  delay of consumer to consume a package\n"
  "  loss_percent percentage of losing a package (0 to 100)\n"
  "\n"
  "Delays are in milliseconds, negatives are maximums for random delays\n";

/**
 * @brief Destructor of ProducerConsumerTest.
 * 
 * Cleans up and deletes all dynamically allocated objects such as producer,
 * dispatcher, consumers, and assembler instances.
 */
ProducerConsumerTest::~ProducerConsumerTest() {
  delete this->producer;
  delete this->dispatcher;
  for ( ConsumerTest* consumer : this->consumers )
    delete consumer;
  delete this->assembler;
}

/**
 * @brief Starts the producer-consumer simulation by parsing arguments and creating simulation objects.
 * 
 * This method creates and initializes the necessary objects like Producer, Dispatcher, Consumers, 
 * and Assembler, sets up their interactions, starts their threads, and waits for them to finish.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Returns EXIT_SUCCESS if the simulation runs successfully, or EXIT_FAILURE if there is an error.
 */
int ProducerConsumerTest::start(int argc, char* argv[]) {
  // Parse arguments and store values in object members
  if ( int error = this->analyzeArguments(argc, argv) ) {
    return error;
  }

  // Create objects for the simulation
  this->producer = new ProducerTest(this->packageCount, this->productorDelay,
                                    this->consumerCount + 1);
  this->dispatcher = new DispatcherTest(this->dispatcherDelay);
  this->dispatcher->createOwnQueue();

  // Create each consumer
  this->consumers.resize(this->consumerCount);
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->consumers[index] = new ConsumerTest(this->consumerDelay);
    assert(this->consumers[index]);
    this->consumers[index]->createOwnQueue();
  }

  // Create the assembler
  this->assembler = new AssemblerTest(this->package_loss_percent,
                                      this->consumerCount);
  this->assembler->createOwnQueue();

  // Communicate simulation objects
  // Producer pushes network messages to the dispatcher queue
  this->producer->setProducingQueue(this->dispatcher->getConsumingQueue());

  // Dispatcher delivers to each consumer, and they should be registered
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->dispatcher->registerRedirect(index + 1,
                                  this->consumers[index]->getConsumingQueue());
  }

  // Redirect assembler's queue with the dispatcher
  this->dispatcher->registerRedirect(this->consumerCount + 1,
                                      this->assembler->getConsumingQueue());
  this->assembler->setProducingQueue(this->dispatcher->getConsumingQueue());

  // Start the simulation threads
  this->producer->startThread();
  this->dispatcher->startThread();
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->consumers[index]->startThread();
  }

  // Start the assembler thread
  this->assembler->startThread();

  // Wait for objects to finish the simulation
  this->producer->waitToFinish();
  this->dispatcher->waitToFinish();
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->consumers[index]->waitToFinish();
  }
  this->assembler->waitToFinish();

  // Simulation finished
  return EXIT_SUCCESS;
}

/**
 * @brief Parses and validates the command-line arguments.
 * 
 * This method extracts the values for package count, consumer count, producer delay, 
 * dispatcher delay, consumer delay, and packet loss percentage from the command-line arguments.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Returns EXIT_SUCCESS if arguments are valid, or EXIT_FAILURE if there are errors.
 */
int ProducerConsumerTest::analyzeArguments(int argc, char* argv[]) {
  // 5 + 2 arguments are mandatory
  if ( argc != 7 ) {
    // Updated to expect 7 arguments
    std::cout << usage;
    return EXIT_FAILURE;
  }

  int index = 1;
  this->packageCount = std::strtoull(argv[index++], nullptr, 10);
  this->consumerCount = std::strtoull(argv[index++], nullptr, 10);
  this->productorDelay = std::atoi(argv[index++]);
  this->dispatcherDelay = std::atoi(argv[index++]);
  this->consumerDelay = std::atoi(argv[index++]);
  this->package_loss_percent = std::atof(argv[index++]);
                              // Convert string to double

  // TODO(Joseaguerrar): Validate that given arguments are valid
  return EXIT_SUCCESS;
}
