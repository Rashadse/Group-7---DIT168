#include <iostream>
#include "visualisation.hpp"
#include <map>

/* This file declares a new OD4 session called visualisation and reroutes messages from the other 3 channels to it. This way, the new channel listens to all messages and can be used by single-viewer in order to visulize those messsages
*/

/**
 * Implementation of the V2VService class as declared in v2v.hpp
 */
VIZService::VIZService(std::string ip, std::string groupId) {
    followerIp = "";
    leaderIp = "";
    myIp = ip;
    myGroupId = groupId;
    

//visualisation OD4Session
// This channel listens to the motor channel, the internal channel and the broadcast channel
// This channel is used for the visualisation microservice 
   visualisation = std::make_shared<cluon::OD4Session>(
        VISUALIZATION_CHANNEL,
        [](cluon::data::Envelope &&/*envelope*/) noexcept {}
    ); // end visualisation declaration



    /*
     * The broadcast field contains a reference to the broadcast channel which is an OD4Session. This is where
     * AnnouncePresence messages will be received.
     */
    broadcast = std::make_shared<cluon::OD4Session>(
            BROADCAST_CHANNEL,
            [this](cluon::data::Envelope &&envelope) noexcept {
                std::cout << "[OD4] ";
                switch (envelope.dataType()) {
                    case ANNOUNCE_PRESENCE: {
                        AnnouncePresence ap = cluon::extractMessage<AnnouncePresence>(std::move(envelope));
						visualisation->send(ap);
                        
                        break;
                    }
                    default: 
                        std::cout << "¯\\_(ツ)_/¯" << std::endl;
                        break;
                }
            });
    
            
    //internal communication
    internalBroadCast = std::make_shared<cluon::OD4Session>(
        INTERNAL_BROADCAST_CHANNEL,
        [this](cluon::data::Envelope &&envelope) noexcept {
            std::cout <<"[INTERNAL BR] ";
            
            
            switch (envelope.dataType()) {

		     // Directed communication
		     case FOLLOW_REQUEST: {
			FollowRequest msg = cluon::extractMessage<FollowRequest>(std::move(envelope));
			visualisation->send(msg);
                             
                         break;
                     }
                     case FOLLOW_RESPONSE: {
			FollowResponse msg = cluon::extractMessage<FollowResponse>(std::move(envelope));
			visualisation->send(msg);

                         break;
                     }
                     case STOP_FOLLOW: {
			StopFollow msg = cluon::extractMessage<StopFollow>(std::move(envelope));
			visualisation->send(msg);

                         break;
                     }
                     case FOLLOWER_STATUS: {
			FollowerStatus msg = cluon::extractMessage<FollowerStatus>(std::move(envelope));
			visualisation->send(msg);

                        break;
                    }
                    case LEADER_STATUS: {
			LeaderStatus msg = cluon::extractMessage<LeaderStatus>(std::move(envelope));	
			visualisation->send(msg);			

                         break;
                     }

		// Internal communication
                case INTERNAL_ANNOUNCE_PRESENCE: {
                    std::cout << "Announcing presence!" << std::endl;

                    InternalAnnouncePresence msg = cluon::extractMessage<InternalAnnouncePresence>(std::move(envelope));
			visualisation->send(msg);
                    break;
                }
                case INTERNAL_FOLLOW_REQUEST: {
                    InternalFollowRequest msg = cluon::extractMessage<InternalFollowRequest>(std::move(envelope));
			visualisation->send(msg);

                    break;
                 }
                 case INTERNAL_STOP_FOLLOW_REQUEST: {
                     InternalStopFollow msg = cluon::extractMessage<InternalStopFollow>(std::move(envelope));
			visualisation->send(msg);

                     break;
                 }
                 case INTERNAL_GET_ALL_GROUPS_REQUEST: {
                     InternalGetAllGroupsRequest msg = cluon::extractMessage<InternalGetAllGroupsRequest>(std::move(envelope));
			visualisation->send(msg);

                break;
                }
                case INTERNAL_EMERGENCY_BRAKE: {
                    InternalEmergencyBrake msg = cluon::extractMessage<InternalEmergencyBrake>(std::move(envelope));
		visualisation->send(msg);

                    break;
                }
                default: 
                    std::cout << "¯\\_(ツ)_/¯" << std::endl;
                    break;
            }
         }
    );
    
    motorBroadcast = std::make_shared<cluon::OD4Session>(
        MOTOR_BROADCAST_CHANNEL,
        [this](cluon::data::Envelope &&envelope) noexcept {
            
            using namespace opendlv::proxy;
            switch (envelope.dataType()) {
                case PEDAL_POSITION_READING: {
                    PedalPositionReading msg = cluon::extractMessage<PedalPositionReading>(std::move(envelope));
					visualisation->send(msg);
                    break;
                }
                case GROUND_STEERING_READING: {
                    GroundSteeringReading msg = cluon::extractMessage<GroundSteeringReading>(std::move(envelope));
					visualisation->send(msg);
                    break;
                }
                default:
                    std::cout << "Received a message that was not understood" << std::endl;
                    break;
            }
        }
    );
}
