#include <iostream>
#include <chrono>
#include "cluon/UDPSender.hpp"
#include "cluon/UDPReceiver.hpp"
#include "PrimeChecker.hpp"

int main(int /*argc*/, char** /*argv*/) {
    PrimeChecker pc;
    std::cout << "Hello, World! " << pc.isPrime(7) << std::endl;
    cluon::UDPSender sender{"127.0.0.1", 1234};  
    sender.send("hello world");

    cluon::UDPReceiver receiver("0.0.0.0", 1235, []
        (std::string &&data, std::string &&/*from*/, std::chrono::system_clock::time_point &&/*time*/) noexcept {
            std::cout << "Received: " << data.size() << std::endl;
        });

    using namespace std::literals::chrono_literals;
    while (receiver.isRunning()) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
