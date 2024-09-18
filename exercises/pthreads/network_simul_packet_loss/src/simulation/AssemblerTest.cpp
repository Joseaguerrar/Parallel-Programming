#include "AssemblerTest.hpp"
#include "Util.hpp"
void AssemblerTest::consume(NetworkMessage data){
    (void)data;
    //Le sumamos la cantidad de mensajes recibidos
    this->receivedMessages++;
    //Un double random en 0 y 100
    double random = 1 + Util::random(0,100);
    if (random> this->loss_probability)
    {
        //Lo mandamos a dormir
        Util::sleepFor(this->producer_delay);
        produce(create_rand_message());
        std::cout<< "Mensaje perdido/Message lost"<< std:: endl;
    }   
}

int AssemblerTest::run(){
    this->consumeForever();
    return EXIT_FAILURE;
}

Queue<NetworkMessage>* AssemblerTest:: getQueue(){
    return this->consumingQueue;
}

NetworkMessage AssemblerTest::create_rand_message() const{
    //Siempre es 1: este productor
    const uint16_t source = this->id;

    int temp_target = 1;

    while (temp_target==id)
    {
        temp_target = Util::random(1,this->consumer_count);
    }

    const uint16_t target = temp_target;
    return NetworkMessage(target,source);
    
}