#include <iostream>

#include "v2v/v2v.hpp"
#include "messages.hpp"


template <class T>
std::string encode(T msg);

/**
 * This is just a simulation program to use before any other group gets their following/leading logic in place. It will
 * create leader updates and can feed them to our car to simulate another vehicle. Note that you will need to uncomment
 * some filtering logic in the incoming UDP receiver in the V2V microservice to use this. The filter removes anything
 * that was sent from an IP that is NOT our leader.
 */

using namespace std;
int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Provide IP of target" << endl;
        exit(1);
    }

    string ip = argv[1]; // For UDP-sender
    
    float currentSpeed = 0;
    float currentSteering = 0;

    using namespace std::chrono;
    cluon::UDPSender *sender = new cluon::UDPSender(ip, 50001);
    int numberOfUpdates = 0; 
    
    /* Initialize following */
    
    cout << "0%        " << std::flush;
    cout << "10%       " << std::flush;
    cout << "20%       " << std::flush;
    cout << "30%       " << std::flush;
    cout << "40%       " << std::flush;
    cout << "50%       " << std::flush;
    cout << "60%       " << std::flush;
    cout << "70%       " << std::flush;
    cout << "80%       " << std::flush;
    cout << "90%       " << std::flush;
    cout << "100%" << std::endl;
    while (numberOfUpdates < 100) { // 12.5 seconds worth of updates
        LeaderStatus ls;
        
        if (numberOfUpdates < 18) {
            currentSpeed += 0.01;    // 1% increments (to ramp up slowly)   
        } else if (numberOfUpdates == 99) {
            currentSpeed = 0;
        }
        
        if (numberOfUpdates > 16 && numberOfUpdates < 24) {
            currentSteering += 0.05; // 5% increments                        
        } else if (numberOfUpdates == 60) {
            currentSteering = 0;
        }
        
        ls.speed(currentSpeed);
        ls.steeringAngle(currentSteering);
        
        sender->send(encode(ls));
        numberOfUpdates += 1;
        
        cout << "|" << std::flush;

        std::this_thread::sleep_for(125ms);
    }
    cout << endl;

}

template <class T>
std::string encode(T msg) {
    cluon::ToProtoVisitor v;
    msg.accept(v);
    std::stringstream buff;
    buff << std::hex << std::setfill('0')
         << std::setw(4) << msg.ID()
         << std::setw(6) << v.encodedData().length()
         << v.encodedData();
    return buff.str();
}
