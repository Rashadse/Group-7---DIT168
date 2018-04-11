#include <iostream>
#include "remotecontrol.hpp"




int main() {
		
	std::shared_ptr<remoteControl> remoteSending = std::make_shared<remoteControl>();

	unsigned char direction;

    while (true) {

	std::cout << "To control the car use" << std::endl;
	std::cout << "           W          " << std::endl;
	std::cout << "    A             D   " << std::endl;
	std::cout << "           S          " << std::endl;
	std::cout << "Press X to emergency stop and quit" << std::endl;

        std::cin >> direction;

                switch (direction) {

                    // accelerates
		    
                    case 'w': {
                        
			remoteSending->Accelerate();
                         
                         break;
           		          }

                    // decelerates
                     case 's': {
                         
                        remoteSending->Decelerate();

                         break;
           		          }
                    // Turns left
                     case 'a': { 

			remoteSending->Left();

                         
                         break;
            		         }
                    // Turns Right
                     case 'd': { 

			remoteSending->Right();
		

                         break;
            		         }
                    // Stops
                     case 'x': {

			remoteSending->Stop();	

                         break;
                		     }
                     default: 
			{
			std::cout << "wrong command, try another command"<< std::endl;

			break;
			}                 
		}
	}
}
remoteControl::remoteControl() {
	
		std::shared_ptr<cluon::OD4Session> motorChannel = std::make_shared<cluon::OD4Session>(MOTOR_CHANNEL,
        	[](cluon::data::Envelope &&envelope) noexcept {	

/*

                switch (envelope.dataType()) {
                    case 4005: {
                      InternalGetAllGroupsResponse response = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
			print response.groupid()
			response.groupid()
				internalChannel->send(response)
                
				  }
                    default: { std::cout << "default case" << std::endl;
                } }

*/

        	});

		std::shared_ptr<cluon::OD4Session> internalChannel = std::make_shared<cluon::OD4Session>(INTERNAL_CHANNEL,
        	[](cluon::data::Envelope &&envelope) noexcept {	

/*                std::cout << "internal channel ";
                switch (envelope.dataType()) {
                    case 4005: {
                	InternalGetAllGroupsResponse response = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
			print response.groupid()
			response.groupid()
				internalChannel->send(response)
                   }
                    default: {std::cout << "¯\\_(ツ)_/¯" << std::endl;
                } } 

*/ 	

//InternalGetAllGroupsRequest message1;

        	});

	}

	opendlv::proxy::GroundSteeringReading msgSteering;
   	opendlv::proxy::PedalPositionReading msgPedal;

	// A variable used to increment/decrement the percentage of the speed pedal
	float pedalPercentage;
				

void remoteControl::Accelerate() {

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

}

void remoteControl::Decelerate() {

			std::cout << "Decelerate" << std::endl;
                        msgSteering.steeringAngle(0.0);
                        motorChannel->send(msgSteering);
                        //the car should decelerate as long as the speed is above 10%. 
                        if (pedalPercentage >= 0.11){
                            pedalPercentage -= 0.1;
                        }

                        std::cout << "Decelerated by" << pedalPercentage << "." << std::endl;
                        msgPedal.percent(pedalPercentage);
                        motorChannel->send(msgPedal);

}

void remoteControl::Left() {

                        std::cout << "Turning left" << std::endl;
                        //the car should slow down before turning if the speed is over 20%.
                        if (pedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
                        motorChannel->send(msgPedal);
			std::cout << "turned left" << pedalPercentage << "." << std::endl;
                        msgSteering.steeringAngle(-15.0);
                        motorChannel->send(msgSteering);

}

void remoteControl::Right() {

                        std::cout << "Turning right" << std::endl;
                        //the car should slow down before turning if the speed is over 20%.
                        if (pedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
                        motorChannel->send(msgPedal);
			std::cout << "turned right" << pedalPercentage << "." << std::endl;
                        msgSteering.steeringAngle(15.0);
                        motorChannel->send(msgSteering);

}

void remoteControl::Stop() {

                        std::cout << "Emergency Stopping" << std::endl;
                        pedalPercentage = 0;
			msgPedal.percent(pedalPercentage);
                        motorChannel->send(msgPedal);
                        msgSteering.steeringAngle(0.0);
                        motorChannel->send(msgSteering);

}
