#include "cluon/UDPSender.hpp"


int main(int /*argc*/, char** /*argv*/) {
    cluon::UDPSender sender{"127.0.0.1", 1235};
    sender.send("Hello receiver!");

    return 0;
}
