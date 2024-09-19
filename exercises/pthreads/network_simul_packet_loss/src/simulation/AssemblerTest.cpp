#include "AssemblerTest.hpp"
#include "Util.hpp"
#include "Log.hpp"

AssemblerTest::AssemblerTest(const double package_loss_percent,
                             const size_t consumer_count)
                             : package_loss_percent(package_loss_percent),
                              consumer_count(consumer_count){
}


int AssemblerTest::run(){
    // Empezar el loop para consumir los mensajes
    this->consumeForever();

    //Cuando termine, imprimir los datos
    Log::append(Log::INFO, "Assembler", std::to_string(this->package_loss_count)
                + "messages lost");
    return EXIT_SUCCESS;
}