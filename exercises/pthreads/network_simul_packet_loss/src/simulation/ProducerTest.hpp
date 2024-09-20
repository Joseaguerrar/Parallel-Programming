/// @copyright 2020 ECCI, Universidad de Costa Rica.
/// All rights reserved. This code is released
/// under the GNU Public License version 3.
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#ifndef PRODUCERTEST_HPP
#define PRODUCERTEST_HPP

#include "NetworkMessage.hpp"
#include "Producer.hpp"

/**
 * @brief Example class of a producer.
 * 
 * This class is responsible for producing network messages and pushing them 
 * to a queue. It simulates the process of message creation with an optional delay
 * and assigns each message a target consumer.
 */
class ProducerTest : public Producer<NetworkMessage> {
  DISABLE_COPY(ProducerTest);
  ///< Macro to disable copy constructor and assignment.

 protected:
  /// Number of packages to be produced.
  size_t packageCount = 0;

  /// Delay of producer to create a package, negative for max random delay.
  int productorDelay = 0;

  /// Number of consumer threads available to receive messages.
  size_t consumerCount = 0;

 public:
  /**
   * @brief Constructor to initialize the producer with simulation parameters.
   * 
   * @param packageCount The number of packages to be produced.
   * @param productorDelay The delay between producing packages (in milliseconds). A negative value indicates a maximum random delay.
   * @param consumerCount The number of consumers that will receive the packages.
   */
  ProducerTest(size_t packageCount, int productorDelay, size_t consumerCount);

  /**
   * @brief Executes the message production process in its own execution thread.
   * 
   * This method is responsible for creating and sending the specified number of messages
   * and signaling the end of production by sending an empty message.
   * 
   * @return int Returns EXIT_SUCCESS upon successful completion of message production.
   */
  int run() override;

  /**
   * @brief Creates a network message to be sent.
   * 
   * This method generates a message with the current index, assigning it a random target
   * consumer and simulating a delay in production.
   * 
   * @param index The index of the message being created.
   * @return NetworkMessage A message containing the source, target, and index.
   */
  NetworkMessage createMessage(size_t index) const;
};

#endif  // PRODUCERTEST_HPP
