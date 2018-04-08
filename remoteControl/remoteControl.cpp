#include <cstdint>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "cluon/OD4Session.hpp"
#include "cluon/Envelope.hpp"

#include "messages.hpp"

int main(int /*argc*/, char** /*argv*/) {

    cluon::OD4Session od4(111,[](cluon::data::Envelope &&envelope) noexcept {
        if (envelope.dataType() == opendlv::proxy::GroundSteeringReading::ID()) {
            opendlv::proxy::GroundSteeringReading receivedMsg = cluon::extractMessage<opendlv::proxy::GroundSteeringReading>(std::move(envelope));
            std::cout << "Sent a message for ground steering to " << receivedMsg.steeringAngle() << "." << std::endl;
        }
        else if (envelope.dataType() == opendlv::proxy::PedalPositionReading::ID()) {
            opendlv::proxy::PedalPositionReading receivedMsg = cluon::extractMessage<opendlv::proxy::PedalPositionReading>(std::move(envelope));
            std::cout << "Sent a message for pedal position to " << receivedMsg.percent() << "." << std::endl;
        }
    });

    if(od4.isRunning() == 0)
    {
        std::cout << "ERROR: No od4 running!!!" << std::endl;
        return -1;
    }
	
	opendlv::proxy::GroundSteeringReading msgSteering;
    opendlv::proxy::PedalPositionReading msgPedal;


    std::cout << "To control the car use" << std::endl;
	std::cout << "           W          " << std::endl;
	std::cout << "    A             D   " << std::endl;
	std::cout << "           S          " << std::endl;
	std::cout << "Press X to emergency stop and quit" << std::endl;

				
				// A variable used to increment/decrement the percentage of the speed pedal
                const int aLittleSpeed = 0.3;
                
				int pedalPercentage = 0;
				
				// A variable used as a condition for the while loop to know when to quit the remote control UI
				bool loop = true;

                while (true) {

                string direction;

                std::cin >> direction;

                switch (direction) {

                    // accelerates
                    case 'w': 
                        
                        std::cout << "Accelerate" << std::endl;
                        msgSteering.steeringAngle(0.0);
                        od4.send(msgSteering);
                        //the car should accelerate as long as the speed is below 40%.
                        if(msgPedal.percent <=40){
                            PedalPercentage += aLittleSpeed;
                        }
                        msgPedal.percent(PedalPercentage);
                        od4.send(msgPedal);
                         
                         break;
                     
                     case 's': 
                         
                        std::cout << "Decelerate" << std::endl;
                        msgSteering.steeringAngle(0.0);
                        od4.send(msgSteering);
                        //the car should decelerate as long as the speed is above 5%. 
                        if(msgPedal.percent >=5){
                            PedalPercentage -= aLittleSpeed;
                        }
                        msgPedal.percent(PedalPercentage);
                        od4.send(msgPedal);

                         break;
                     
                     case 'a': 
                        std::cout << "Turning left" << std::endl;
                        //the car should slow down before turning if the speed is over 20%.
                        if (PedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
                        od4.send(msgPedal);
                        msgSteering.steeringAngle(-15.0);
                        od4.send(msgSteering);
                         
                         break;
                     
                     case 'd': 
                        std::cout << "Turning right" << std::endl;
                        //the car should slow down before turning if the speed is over 20%.
                        if (PedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
                        od4.send(msgPedal);
                        msgSteering.steeringAngle(15.0);
                        od4.send(msgSteering);

                         break;
                     
                     case 'x': 
                        std::cout << "Emergency Stopping" << std::endl;
                        pedalPercentage = 0.0;
                        od4.send(msgPedal);
                        msgSteering.steeringAngle(0.0);
                        od4.send(msgSteering);
						std::cout << "quitting" << std::endl;
						loop = false;
                         break;
                     
                     default: 
						cout << "wrong command, the car will now stop"<< endl;
						pedalPercentage = 0.0;
						od4.send(msgPedal);
						msgSteering.steeringAngle(0.0);
						od4.send(msgSteering);
                 }
			}
		}


