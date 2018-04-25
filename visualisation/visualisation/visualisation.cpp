#include <iostream>
#include "visualisation.hpp"
#include <map>

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
                    
                    std::cout << "received '" << msg.LongName() << std::endl;
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

    /*
     * Each car declares an incoming UDPReceiver for messages directed at them specifically. This is where messages
     * such as FollowRequest, FollowResponse, StopFollow, etc. are received.
     */
    incoming = std::make_shared<cluon::UDPReceiver>(
            "0.0.0.0",
            DEFAULT_PORT,
            [this](std::string &&data, std::string &&sender, 			std::chrono::system_clock::time_point /*&&ts*/) noexcept {
                std::cout << "[UDP] ";
                std::pair<int16_t, std::string> msg = extract(data);

		        std::string senderIp = sender.substr(0, sender.find(":"));

                switch (msg.first) {
                    case FOLLOW_REQUEST: {
                        FollowRequest followRequest = decode<FollowRequest>(msg.second);
			visualisation->send(followRequest);
                             
                         break;
                     }
                     case FOLLOW_RESPONSE: {
                         FollowResponse followResponse = decode<FollowResponse>(msg.second);
			visualisation->send(followResponse);

                         break;
                     }
                     case STOP_FOLLOW: {
                         StopFollow stopFollow = decode<StopFollow>(msg.second);
			visualisation->send(stopFollow);

                         break;
                     }
                     case FOLLOWER_STATUS: {
                         FollowerStatus followerStatus = decode<FollowerStatus>(msg.second);
			visualisation->send(followerStatus);

                        break;
                    }
                    case LEADER_STATUS: {
                        LeaderStatus leaderStatus = decode<LeaderStatus>(msg.second);
			visualisation->send(leaderStatus);			

                         break;
                     }
                     default: std::cout << "¯\\_(ツ)_/¯" << std::endl;
                 }
             });
}




/**
 * The extraction function is used to extract the message ID and message data into a pair.
 *
 * @param data - message data to extract header and data from
 * @return pair consisting of the message ID (extracted from the header) and the message data
 */
std::pair<int16_t, std::string> VIZService::extract(std::string data) {
    if (data.length() < 10) return std::pair<int16_t, std::string>(-1, "");
    unsigned int id, len;
    std::stringstream ssId(data.substr(0, 4));
    std::stringstream ssLen(data.substr(4, 10));
    ssId >> std::hex >> id;
    ssLen >> std::hex >> len;
    return std::pair<int16_t, std::string> (
            data.length() -10 == len ? id : -1,
            data.substr(10, data.length() -10)
    );
};

/**
 * Generic encode function used to encode a message before it is sent.
 *
 * @tparam T - generic message type
 * @param msg - message to encode
 * @return encoded message
 */
template <class T>
std::string VIZService::encode(T msg) {
    cluon::ToProtoVisitor v;
    msg.accept(v);
    std::stringstream buff;
    buff << std::hex << std::setfill('0')
         << std::setw(4) << msg.ID()
         << std::setw(6) << v.encodedData().length()
         << v.encodedData();
    return buff.str();
}

/**
 * Generic decode function used to decode an incoming message.
 *
 * @tparam T - generic message type
 * @param data - encoded message data
 * @return decoded message
 */
template <class T>
T VIZService::decode(std::string data) {
    std::stringstream buff(data);
    cluon::FromProtoVisitor v;
    v.decodeFrom(buff);
    T tmp = T();
    tmp.accept(v);
    return tmp;
}
