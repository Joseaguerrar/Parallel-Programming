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
    int loss_probability = 0;
    int consumer_count = 0;
    // Delay del consumidor para consumir un paquete
    int producer_delay = 0;
    // Numbero de mensajes recibidos
    size_t receivedMessages = 0;
    int id = 0;
  public:
    //Constructor
    explicit AssemblerTest(int loss_probability, int consumer_count, int producer_delay, int id):Assembler() {
    this->loss_probability = loss_probability;
    this->consumer_count = consumer_count;
    this->producer_delay = producer_delay;
    this->id = id;
    }
    //Destructor
    virtual ~AssemblerTest(){
    }
    int run() override;

    //Consumir producto
    void consume(NetworkMessage data) override;
    NetworkMessage createMessageRand() const;
    Queue<NetworkMessage>* getQueue();
};

#endif