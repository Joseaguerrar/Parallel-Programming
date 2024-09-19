#include "AssemblerTest.hpp"
#include "Util.hpp"
#include "Log.hpp"

/**
 * @brief Constructs an AssemblerTest object with specified packet loss percentage and consumer count.
 * 
 * @param package_loss_percent Percentage of packets expected to be lost.
 * @param consumer_count Number of consumers on the network.
 */
AssemblerTest::AssemblerTest(const double package_loss_percent,
                             const size_t consumer_count)
                             : package_loss_percent(package_loss_percent),
                               consumer_count(consumer_count) {
}

/**
 * @brief Starts the message consumption process in a continuous loop.
 * 
 * This method initiates the process of consuming messages indefinitely by calling 
 * `consumeForever()` and logs the number of lost messages when the process completes.
 * 
 * @return int Status code, returns EXIT_SUCCESS on completion.
 */
int AssemblerTest::run() {
    // Start consuming messages in an infinite loop
    this->consumeForever();

    // When finished, log the total number of lost messages
    Log::append(Log::INFO, "Assembler", std::to_string(this->package_loss_count)
                + " messages lost");
    return EXIT_SUCCESS;
}

/**
 * @brief Consumes a network message and simulates packet loss.
 * 
 * This method checks if the message should be discarded based on the packet loss percentage. 
 * If not discarded, it assigns a random target consumer and forwards the message.
 * 
 * @param data The NetworkMessage object that is to be consumed.
 */
void AssemblerTest::consume(NetworkMessage data) {
    (void)data;  // Explicitly ignore the input data for now.

    // Simulate packet loss based on the specified loss percentage
    if (Util::random(0.0, 100.0) < this->package_loss_percent) {
        this->package_loss_count++;  // Increment the lost packet count
    } else {
        // Assign a random consumer target to the message
        data.target = Util::random(1, this->consumer_count + 1);
        // Forward the message to the next stage (producer)
        this->produce(data);
    }
}
