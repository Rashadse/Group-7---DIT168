#include <cstdint>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "cluon/OD4Session.hpp"
#include "cluon/Envelope.hpp"

#include "messages.hpp"


int main(int /*argc*/, char** /*argv*/) {


std::shared_ptr<cluon::OD4Session> motorChannel = std::make_shared<cluon::OD4Session>(
            180,
            [](cluon::data::Envelope &&envelope) noexcept {
                switch (envelope.dataType()) {
                    case 4005: {
                //        InternalGetAllGroupsResponse response = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
			//print response.groupid()
//			response.groupid()
//				internalChannel->send(response)
                    }
                    default: std::cout << "default case" << std::endl;
                }
            });

//std::shared_ptr<cluon::OD4Session> internalChannel = std::make_shared<cluon::OD4Session>(
//            181,
//            [](cluon::data::Envelope &&envelope) noexcept {
//                std::cout << "[OD4] ";
//                switch (envelope.dataType()) {
//                    case 4005: {
                //        InternalGetAllGroupsResponse response = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
			//print response.groupid()
//			response.groupid()
//				internalChannel->send(response)
//                   }
//                    default: std::cout << "¯\\_(ツ)_/¯" << std::endl;
//                }
//            });

	opendlv::proxy::GroundSteeringReading msgSteering;
   	opendlv::proxy::PedalPositionReading msgPedal;
	//InternalGetAllGroupsRequest message1;
	
		
		
	// A variable used to increment/decrement the percentage of the speed pedal
	float pedalPercentage;
				
	// A variable used as a condition for the while loop to know when to quit the remote 		control UI
	bool loop = true;

                while (loop) {

                char direction;
	
	std::cout << "To control the car use" << std::endl;
	std::cout << "           W          " << std::endl;
	std::cout << "    A             D   " << std::endl;
	std::cout << "           S          " << std::endl;
	std::cout << "Press X to emergency stop and quit" << std::endl;


		this_thread::sleep_for(chrono::milliseconds(2000));

                std::cin >> direction;


                switch (direction) {

                    // accelerates
		    
                    case 'w': {
                        
                        std::cout << "Accelerate" << std::endl;
                        msgSteering.steeringAngle(0.0);
                        motorChannel->send(msgSteering);
                        //the car should accelerate as long as the speed is below 40%.
                	if (pedalPercentage <= 0.4){
                            pedalPercentage += 0.1;
                        }
			std::cout << "Accelerated by" << pedalPercentage << "." << std::endl;
                        msgPedal.percent(pedalPercentage);
                        motorChannel->send(msgPedal);
                         
                         break;
           		          }
                     case 's': {
                         
                        std::cout << "Decelerate" << std::endl;
                        msgSteering.steeringAngle(0.0);
                        motorChannel->send(msgSteering);
                        //the car should decelerate as long as the speed is above 10%. 
                        if (pedalPercentage >= 0.11){
                            pedalPercentage -= 0.1;
                        }

                        std::cout << "Accelerated by" << pedalPercentage << "." << std::endl;
                        msgPedal.percent(pedalPercentage);
                        motorChannel->send(msgPedal);

                         break;
           		          }
                     case 'a': { 
                        std::cout << "Turning left" << std::endl;
                        //the car should slow down before turning if the speed is over 20%.
                        if (pedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
                        motorChannel->send(msgPedal);
			std::cout << "turned left" << pedalPercentage << "." << std::endl;
                        msgSteering.steeringAngle(-15.0);
                        motorChannel->send(msgSteering);
                         
                         break;
            		         }
                     case 'd': { 
                        std::cout << "Turning right" << std::endl;
                        //the car should slow down before turning if the speed is over 20%.
                        if (pedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
                        motorChannel->send(msgPedal);
			std::cout << "turned right" << pedalPercentage << "." << std::endl;
                        msgSteering.steeringAngle(15.0);
                        motorChannel->send(msgSteering);

                         break;
            		         }
                     case 'x': {
                        std::cout << "Emergency Stopping" << std::endl;
                        pedalPercentage = 0;
			msgPedal.percent(pedalPercentage);
                        motorChannel->send(msgPedal);
                        msgSteering.steeringAngle(0.0);
                        motorChannel->send(msgSteering);
			std::cout << "quitting" << std::endl;
			loop = false;
                         break;
                		     }
                     default: 
			{
			std::cout << "wrong command, the car will now stop"<< std::endl;
			pedalPercentage = 0;
			msgPedal.percent(pedalPercentage);
			motorChannel->send(msgPedal);
			msgSteering.steeringAngle(0.0);
			motorChannel->send(msgSteering);
			}                 
		}
	}
return 0;
}


