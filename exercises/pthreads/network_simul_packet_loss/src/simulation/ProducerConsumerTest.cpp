/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#include <cstdlib>
#include <iostream>

#include "ProducerConsumerTest.hpp"
#include "ConsumerTest.hpp"
#include "DispatcherTest.hpp"
#include "ProducerTest.hpp"
#include "AssemblerTest.hpp"

const char* const usage =
  "Usage: prodcons packages consumers prod_delay disp_delay cons_delay\n"
  "\n"
  "  packages    number of packages to be produced\n"
  "  consumers   number of consumer threads\n"
  "  prod_delay  delay of producer to create a package\n"
  "  disp_delay  delay of dispatcher to dispatch a package\n"
  "  cons_delay  delay of consumer to consume a package\n"
  "  loss_percent percentage of lossing a package (0 to 100)\n"
  "\n"
  "Delays are in millisenconds, negatives are maximums for random delays\n";

ProducerConsumerTest::~ProducerConsumerTest() {
  delete this->producer;
  delete this->dispatcher;
  for ( ConsumerTest* consumer : this->consumers )
    delete consumer;
  delete this->assembler;
}

int ProducerConsumerTest::start(int argc, char* argv[]) {
  // Parse arguments and store values in object members
  if ( int error = this->analyzeArguments(argc, argv) ) {
    return error;
  }

  // Create objects for the simulation
  this->producer = new ProducerTest(this->packageCount, this->productorDelay
    , this->consumerCount+1);
  this->dispatcher = new DispatcherTest(this->dispatcherDelay);
  this->dispatcher->createOwnQueue();
  // Create each producer
  this->consumers.resize(this->consumerCount);
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->consumers[index] = new ConsumerTest(this->consumerDelay);
    assert(this->consumers[index]);
    this->consumers[index]->createOwnQueue();
  }
  //Creamos el assembler
  this->assembler=new AssemblerTest(this->package_loss_percent,
                                    this->consumerCount);
  //Creamos su queue
  this->assembler->createOwnQueue();
  // Communicate simulation objects
  // Producer push network messages to the dispatcher queue
  this->producer->setProducingQueue(this->dispatcher->getConsumingQueue());
  // Dispatcher delivers to each consumer, and they should be registered
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->dispatcher->registerRedirect(index + 1
      , this->consumers[index]->getConsumingQueue());
  }
  //Redirigimos la cola del assembler con la del dispatcher
  this->dispatcher->registerRedirect(this->consumerCount+1,
                                     this->assembler->getConsumingQueue());
  this->assembler->setProducingQueue(this->dispatcher->getConsumingQueue());
  // Start the simulation
  this->producer->startThread();
  this->dispatcher->startThread();
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->consumers[index]->startThread();
  }
  //Ejecutamos el assembler
  this->assembler->startThread();
  // Wait for objets to finish the simulation
  this->producer->waitToFinish();
  this->dispatcher->waitToFinish();
  for ( size_t index = 0; index < this->consumerCount; ++index ) {
    this->consumers[index]->waitToFinish();
  }
  this->assembler->waitToFinish();

  // Simulation finished
  return EXIT_SUCCESS;
}

int ProducerConsumerTest::analyzeArguments(int argc, char* argv[]) {
  // 5 + 2 arguments are mandatory
  if ( argc != 7 ) {
    //Ahora son 7 argumentos
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
  //atof es de double

  // todo: Validate that given arguments are fine
  return EXIT_SUCCESS;
}
