#ifndef ASSEMBLER_TEST_HPP
#define ASSEMBLER_TEST_HPP

#include "Assembler.hpp"
#include "NetworkMessage.hpp"
 /**
 * @brief A AssemblerTest class example
 */
class AssemblerTest : public Assembler<NetworkMessage, NetworkMessage> {
  DISABLE_COPY(AssemblerTest);

 protected:
  /// Percent of package loss
  double packageLossPercent = 0.0;
  /// Amount of consumer on the network
  size_t consumerCount = 0;
  /// Amount of message lost
  size_t packageLossCount = 0;
};
#endif