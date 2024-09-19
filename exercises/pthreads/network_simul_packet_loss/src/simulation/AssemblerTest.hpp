#ifndef ASSEMBLER_TEST_HPP
#define ASSEMBLER_TEST_HPP

#include "Assembler.hpp"
#include "NetworkMessage.hpp"

/**
 * @brief A AssemblerTest class example.
 * 
 * This class inherits from the Assembler template and is specialized for 
 * handling NetworkMessage objects. It introduces additional functionality 
 * to simulate packet loss in a network and counts the number of lost packets.
 */
class AssemblerTest : public Assembler<NetworkMessage, NetworkMessage> {
  DISABLE_COPY(AssemblerTest);

 protected:
  /// Percent of package loss during message transmission.
  double package_loss_percent = 0.0;
  
  /// Number of consumers connected to the network.
  size_t consumer_count = 0;
  
  /// Number of lost packages due to simulated network conditions.
  size_t package_loss_count = 0;

 public:
  /**
   * @brief Constructs an AssemblerTest object with specified loss and consumer count.
   * 
   * @param package_loss_percent Percentage of packets expected to be lost during transmission.
   * @param consumer_count Number of consumers on the network.
   */
  explicit AssemblerTest(const double package_loss_percent,
                         const size_t consumer_count);

  /**
   * @brief Runs the assembler's message consumption process in a dedicated thread.
   * 
   * This method starts the message processing and is responsible for handling
   * network message consumption and simulating packet loss.
   * 
   * @return int Status code of the execution.
   */
  int run() override;

  /**
   * @brief Consumes a network message, simulating processing and potential loss.
   * 
   * This method is called when a new NetworkMessage is received. Depending on
   * the specified packet loss percentage, the message might be discarded.
   * 
   * @param data The network message to be consumed.
   */
  void consume(NetworkMessage data) override;
};

#endif
