#include <iostream>
#include <chrono>

#include "cluon/UDPSender.hpp"
#include "cluon/UDPReceiver.hpp"
#include "cluon/ToProtoVisitor.hpp"
#include "cluon/FromProtoVisitor.hpp"
#include "PrimeChecker.hpp"
#include "messages.hpp"

int main(int /*argc*/, char** /*argv*/) {
    PrimeChecker pc;
    std::cout << "Hello, World! " << pc.isPrime(7) << std::endl;
    
    cluon::UDPSender sender{"225.0.0.111", 1238};
    
    // Take input from user of number to check if prime.
    uint16_t value;
    std::cout << "Enter a number to check if it is a prime one: ";
    std::cin >> value;
    Handshake msg;
    msg.entity(value);
    
    // Encode!
    cluon::ToProtoVisitor encoder;
    msg.accept(encoder);
    std::string data{encoder.encodedData()};
    
    // Send!
    sender.send(std::move(data));
    
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(5s);

    cluon::UDPReceiver receiver("225.0.0.111", 1238, []
        (std::string &&data, std::string &&/*from*/, std::chrono::system_clock::time_point &&/*time*/) noexcept {
            // ProtoVisitors can only take a stringstream as input for decoding.
            std::stringstream sstr(data);
            // Decode the incoming message.
            cluon::FromProtoVisitor decoder;
            decoder.decodeFrom(sstr);
            Handshake msg;
            msg.accept(decoder);
            // Check if received message is prime and print result.
            PrimeChecker pc;
            std::cout << msg.entity() << " is " << (pc.isPrime(msg.entity()) ? "" : "not ") << "a prime" << std::endl;
        });

    using namespace std::literals::chrono_literals;
    while (receiver.isRunning()) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
