#include <iostream>

#include "v2v/v2v.hpp"
#include "messages.hpp"


template <class T>
std::string encode(T msg);


using namespace std;
int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Provide IP of target" << endl;
        exit(1);
    }

    string ip = argv[1]; // For UDP-sender
    
    float currentSpeed = 0;
    float currentSteering = 0;


    while (true) {
        unsigned int choice;
        cout << "Leader control menu" << endl;
        cout << "(1) Start" << endl;
        cout << ">> ";
        cin >> choice;
        
     
        switch (choice) {
            case 1: {
                using namespace std::chrono;
                cluon::UDPSender *sender = new cluon::UDPSender(ip, 50001);
                int numberOfUpdates = 0; 
                
                /* Initialize following */
                
                while (numberOfUpdates < 100) { // roughly 15 seconds worth of updates
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
                
                break;
            }
            default: {
                return 0;
            }
        }
    }
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
