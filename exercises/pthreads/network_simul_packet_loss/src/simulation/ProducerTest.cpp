/// @copyright 2020 ECCI, Universidad de Costa Rica.
/// All rights reserved. This code is released
/// under the GNU Public License version 3.
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#include "ProducerTest.hpp"
#include "Log.hpp"
#include "Util.hpp"

/**
 * @brief Constructs a ProducerTest object.
 * 
 * This constructor initializes the producer with the number of packages to create, 
 * the delay for producing each message, and the number of consumers to send messages to.
 * 
 * @param packageCount The number of messages (packages) to produce.
 * @param productorDelay The delay for each message creation (in milliseconds).
 * @param consumerCount The number of consumers the producer can send messages to.
 */
ProducerTest::ProducerTest(size_t packageCount,
                           int productorDelay, size_t consumerCount)
  : packageCount(packageCount)
  , productorDelay(productorDelay)
  , consumerCount(consumerCount) {
}

/**
 * @brief Starts the production process of network messages.
 * 
 * This method loops through the number of packages specified during construction, 
 * creates and sends each message, and signals the end of production by sending an 
 * empty message.
 * 
 * @return int Returns EXIT_SUCCESS upon successful completion of production.
 */
int ProducerTest::run() {
  // Produce each requested message
  for ( size_t index = 0; index < this->packageCount; ++index ) {
    this->produce(this->createMessage(index));
  }

  // Produce an empty message to signal completion
  this->produce(NetworkMessage());

  // Log that production has completed
  Log::append(Log::INFO, "Producer", std::to_string(this->packageCount)
    + " messages sent");
  return EXIT_SUCCESS;
}

/**
 * @brief Creates a network message.
 * 
 * This method generates a network message with the current index. The source of the message
 * is always the producer (source = 1), and the target is randomly selected from the available consumers.
 * The delay for producing each message is simulated with a sleep.
 * 
 * @param index The current index of the message being produced.
 * @return NetworkMessage A message containing the source, target, and index.
 */
NetworkMessage ProducerTest::createMessage(size_t index) const {
  // Source is always 1: this producer
  const uint16_t source = 1;

  // Target is selected randomly from the consumers
  const uint16_t target = 1 + Util::random(0,
                                         static_cast<int>(this->consumerCount));

  // Simulate the delay for producing the message (using sleep)
  Util::sleepFor(this->productorDelay);

  // Create and return a copy of the network message
  return NetworkMessage(target, source, index);
}
