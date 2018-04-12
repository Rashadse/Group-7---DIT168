#include <iostream>
#include "remotecontrol.hpp"


	opendlv::proxy::GroundSteeringReading msgSteering;
   	opendlv::proxy::PedalPositionReading msgPedal;


	
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

	


int main() {
		
	std::shared_ptr<remoteControl> remoteSending = std::make_shared<remoteControl>();
	this_thread::sleep_for(chrono::milliseconds(2000));
	

    while (true) {

	this_thread::sleep_for(chrono::milliseconds(2000));

	std::cout << "          -----             " << std::endl;
	std::cout << "                                  " << std::endl;

	std::cout << "To control the car use" << std::endl;
	std::cout << "           1          " << std::endl;
	std::cout << "    3             4   " << std::endl;
	std::cout << "           2          " << std::endl;
	std::cout << "Press 0 to emergency stop and quit" << std::endl;
	std::cout << "                                  " << std::endl;
	unsigned int direction;
        std::cin >> direction;

                switch (direction) {

                    // accelerates
		    
                    case 1: {
                        
			remoteSending->Accelerate();
                        motorChannel->send(msgSteering);
                        motorChannel->send(msgPedal);
                         
                         break;
           		          }

                    // decelerates
                     case 2: {
                         
                        remoteSending->Decelerate();
                        motorChannel->send(msgSteering);
                        motorChannel->send(msgPedal);

                         break;
           		          }
                    // Turns left
                     case 3: { 

			remoteSending->Left();
                        motorChannel->send(msgSteering);
                        motorChannel->send(msgPedal);
                         
                         break;
            		         }
                    // Turns Right
                     case 4: { 

			remoteSending->Right();
                        motorChannel->send(msgSteering);
                        motorChannel->send(msgPedal);

                         break;
            		         }
                    // Stops
                     case 0: {

			remoteSending->Stop();
                        motorChannel->send(msgSteering);
                        motorChannel->send(msgPedal);
			return -1;


                		     }
                     default: 
			{
			std::cout << "wrong command, try another command"<< std::endl;

			break;
			}                 
		}
	}
	return 0;
}



	// A variable used to increment/decrement the percentage of the speed pedal
	float pedalPercentage;
				

void remoteControl::Accelerate() {

                        msgSteering.steeringAngle(0.0);
                        //the car should accelerate as long as the speed is below 30%.
                	if (pedalPercentage <= 0.3){
                            pedalPercentage += 0.05;
                        }else{std::cout << "You have reached maximum speed" << std::endl;
			}

			std::cout << "Accelerated to " << pedalPercentage * 100 << "%." << std::endl;
                        msgPedal.percent(pedalPercentage);


}

void remoteControl::Decelerate() {

                        msgSteering.steeringAngle(0.0);
 
                        //the car should decelerate as long as the speed is above 10%. 
                        if (pedalPercentage >= 0.15){
                            pedalPercentage -= 0.05;
                        }

                        std::cout << "Decelerated to " << pedalPercentage * 100 << "%." << std::endl;
                        msgPedal.percent(pedalPercentage);

}

void remoteControl::Left() {

                        //the car should slow down before turning if the speed is over 20%.
                        if (pedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
			std::cout << "turned left at new speed: " << pedalPercentage * 100 << "%." << std::endl;
                        msgSteering.steeringAngle(-15.0);

}

void remoteControl::Right() {

                        //the car should slow down before turning if the speed is over 20%.
                        if (pedalPercentage >= 0.2){
                        pedalPercentage = 0.2;}
			std::cout << "turned right at speed: " << pedalPercentage * 100 << "%." << std::endl;
                        msgSteering.steeringAngle(15.0);

}

void remoteControl::Stop() {

                        pedalPercentage = 0;
			msgPedal.percent(pedalPercentage);
                        msgSteering.steeringAngle(0.0);
                        std::cout << "Stopping" << std::endl;
}
