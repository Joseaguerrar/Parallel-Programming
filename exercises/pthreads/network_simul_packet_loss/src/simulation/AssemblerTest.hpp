#ifndef ASSEMBLER_TEST_HPP
#define ASSEMBLER_TEST_HPP

#include <iostream>
#include "Assembler.hpp"
#include "NetworkMessage.hpp"
#include "Queue.hpp"
class AssemblerTest  
  :public Assembler<NetworkMessage, NetworkMessage> {
  DISABLE_COPY(AssemblerTest);
  protected: 
  // Numeros de mensajes recibidos
  int lossProbability = 0;
  int consumerCount = 0;
  // Delay del consumidor para consumir un paquete
  int producerDelay = 0;
  // Numbero de mensajes recibidos
  size_t receivedMessages = 0;
  int id = 0;
  
};

#endif