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
  double package_loss_percent = 0.0;
  /// Amount of consumer on the network
  size_t consumer_count = 0;
  /// Amount of message lost
  size_t package_loss_count = 0;
 public:
 ///Constructor
  explicit AssemblerTest(const double package_loss_percent,
                        const size_t consumer_count);
 ///Consumir los mensajes en su propio hilo
 int run() override;

 ///Override para cualquier tipo de dato
 void consume(NetworkMessage data) override;
};
#endif