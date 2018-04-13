#include <iostream>
#include "remotecontrol.hpp"
using namespace std;

	

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



        	});

		std::shared_ptr<cluon::OD4Session> internalChannel = std::make_shared<cluon::OD4Session>(INTERNAL_CHANNEL,
        	[](cluon::data::Envelope &&envelope) noexcept {	

---------

               std::cout << "internal channel ";
                switch (envelope.dataType()) {
                    case 4005: {
                	InternalGetAllGroupsResponse response = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
			print response.groupid()
			response.groupid()
				internalChannel->send(response)
                   }
                    default: {std::cout << "¯\\_(ツ)_/¯" << std::endl;
                } } 


//InternalGetAllGroupsRequest message1;

        	});
*/
	
    cluon::OD4Session motorChannel(MOTOR_CHANNEL,[](cluon::data::Envelope &&envelope) noexcept {
        if (envelope.dataType() == opendlv::proxy::GroundSteeringReading::ID()) {
            opendlv::proxy::GroundSteeringReading receivedMsg = cluon::extractMessage<opendlv::proxy::GroundSteeringReading>(std::move(envelope));
            std::cout << "Sent a message for ground steering to " << receivedMsg.steeringAngle() << "." << std::endl;
        }
        else if (envelope.dataType() == opendlv::proxy::PedalPositionReading::ID()) {
            opendlv::proxy::PedalPositionReading receivedMsg = cluon::extractMessage<opendlv::proxy::PedalPositionReading>(std::move(envelope));
            std::cout << "Sent a message for pedal position to " << receivedMsg.percent() << "." << std::endl;
        }
    });


	opendlv::proxy::GroundSteeringReading msgSteering;
   	opendlv::proxy::PedalPositionReading msgPedal;

int main() {
		
	std::shared_ptr<remoteControl> remoteSending = std::make_shared<remoteControl>();
	
	unsigned char direction;

	std::cout << "          -----             " << std::endl;
	std::cout << "                            " << std::endl;
	std::cout << "To control the car use" << std::endl;
	std::cout << "           w          " << std::endl;
	std::cout << "    a             d   " << std::endl;
	std::cout << "           s          " << std::endl;
	std::cout << "Press x to emergency stop and quit" << std::endl;
	std::cout << "                                  " << std::endl;
   
   while (true) {

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	
        std::cin >> direction;

                switch (direction) {

                    // accelerates
		    
                    case 'w': {
                        
			remoteSending->Accelerate();
                        motorChannel.send(msgSteering);
                        motorChannel.send(msgPedal);
                         
                         break;
           		          }

                    // decelerates
                     case 's': {
                         
                        remoteSending->Decelerate();
                        motorChannel.send(msgSteering);
                        motorChannel.send(msgPedal);

                         break;
           		          }
                    // Turns left
                     case 'a': { 

			remoteSending->Left();
                        motorChannel.send(msgSteering);
                        motorChannel.send(msgPedal);
                         
                         break;
            		         }
                    // Turns Right
                     case 'd': { 

			remoteSending->Right();
                        motorChannel.send(msgSteering);
                        motorChannel.send(msgPedal);

                         break;
            		         }
                    // Stops
                     case 'x': {

			remoteSending->Stop();
                        motorChannel.send(msgSteering);
                        motorChannel.send(msgPedal);
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
