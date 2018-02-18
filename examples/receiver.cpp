#include <chrono>
#include <iostream>

#include "cluon/UDPReceiver.hpp"

int main(int /*argc*/, char** /*argv*/) {
    cluon::UDPReceiver receiver("0.0.0.0", 1235, []
        (std::string &&data, std::string &&/*from*/, std::chrono::system_clock::time_point &&/*timepoint*/) noexcept {
            std::cout << "Received: " << data << std::endl;
        }
    );
    
    using namespace std::literals::chrono_literals;
    while (receiver.isRunning()) {
        std::this_thread::sleep_for(1s);
    }
    
    return 0;
}
