#include <iostream>
#include <chrono>
#include "cluon/UDPSender.hpp"
#include "cluon/UDPReceiver.hpp"
#include "PrimeChecker.hpp"

int main(int /*argc*/, char** /*argv*/) {
    PrimeChecker pc;
    std::cout << "Hello, World! " << pc.isPrime(7) << std::endl;
    
    cluon::UDPSender sender{"225.0.0.111", 1236};
    sender.send("hello world");
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(5s);

    cluon::UDPReceiver receiver("225.0.0.111", 1236, []
        (std::string &&data, std::string &&/*from*/, std::chrono::system_clock::time_point &&/*time*/) noexcept {
            std::cout << "Received: " << data.size() << std::endl;
        });

    using namespace std::literals::chrono_literals;
    while (receiver.isRunning()) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
