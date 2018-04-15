#include <iostream>

#include "cluon/OD4Session.hpp"
#include "cluon/Envelope.hpp"

#include "v2v/v2v.hpp"
#include "messages.hpp"


using namespace std;
int main() {
    shared_ptr<cluon::OD4Session> internalBroadcast = make_shared<cluon::OD4Session>(
        INTERNAL_BROADCAST_CHANNEL,
        [](cluon::data::Envelope &&envelope) noexcept {
            cout << "[RC INTERNAL] ";
            
            switch (envelope.dataType()) {
                case INTERNAL_FOLLOW_RESPONSE: {
                    InternalFollowResponse msg = cluon::extractMessage<InternalFollowResponse>(std::move(envelope));
                    
                    cout << "Now following group: " << msg.groupid() << " status: " << msg.status() << endl;
                    break;
                }
                case INTERNAL_STOP_FOLLOW_RESPONSE: {
                    InternalStopFollowResponse msg = cluon::extractMessage<InternalStopFollowResponse>(std::move(envelope));
                    
                    cout << "Stopped following group: " << msg.groupid() << endl;
                    break;
                }
                case INTERNAL_GET_ALL_GROUPS_RESPONSE: {
                    InternalGetAllGroupsResponse msg = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
                    
                    cout << "Got group: " << msg.groupid() << endl;
                    break;
                }
                case INTERNAL_EMERGENCY_BRAKE: {
                    InternalEmergencyBrake msg = cluon::extractMessage<InternalEmergencyBrake>(std::move(envelope));
                    
                    cout << "Emergency brake!" << endl;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    );
    
    shared_ptr<cluon::OD4Session> motorBroadcast = make_shared<cluon::OD4Session>(
        MOTOR_BROADCAST_CHANNEL,
        [](cluon::data::Envelope &&envelope) noexcept {
            std::cout << "[RC MOTOR] ";
            
            using namespace opendlv::proxy;
            switch (envelope.dataType()) {
                case PEDAL_POSITION_READING: {
                    PedalPositionReading msg = cluon::extractMessage<PedalPositionReading>(std::move(envelope));
                    
                    std::cout << "Got new pedal position: " << msg.percent() << std::endl;
                    break;
                }
                case GROUND_STEERING_READING: {
                    GroundSteeringReading msg = cluon::extractMessage<GroundSteeringReading>(std::move(envelope));
                    
                    std::cout << "Got new steering reading: " << msg.steeringAngle() << std::endl;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    );
    float currentSpeed = 0.0;
    float currentSteering = 0.0;


    while (true) {
        unsigned int choice;
        cout << "Which message would you like to send?" << endl;
        cout << "(1) InternalFollow" << endl;
        cout << "(2) InternalStopFollow" << endl;
        cout << "(3) InternalGetAllGroups" << endl;
        cout << "(4) InternalEmergencyBrake" << endl;
        cout << "(5) Increase speed!" << endl;
        cout << "(6) Decrease speed!" << endl;
        cout << "(7) Turn left!" << endl;
        cout << "(8) Turn right!" << endl;
        cout << "(9) Announce your presence!" << endl;
        cout << "(#) Nothing, just quit." << endl;
        cout << ">> ";
        cin >> choice;
        
     
        switch (choice) {
            case 1: {
                InternalFollowRequest msg;
                std::string groupId = "";
                
                cout << "Which vehicle would you like to follow?" << endl;
                cin >> groupId;
                
                msg.groupid(groupId);
                internalBroadcast->send(msg);
                break;
            }
            case 2: {
                InternalStopFollow msg;
                std::string groupId = "";
                
                cout << "Which vehicle would you like to stop following?" << endl;
                cin >> groupId;
                
                msg.groupid(groupId);
                internalBroadcast->send(msg);
                break;
            }
            case 3: { 
                InternalGetAllGroupsRequest msg;
                internalBroadcast->send(msg);                   
                break;
            }
            case 4: {
                InternalEmergencyBrake msg;
                internalBroadcast->send(msg);
                break;
            }
            case 5: {
                if (currentSpeed < 0.25) {
                    currentSpeed += 0.05;
                }   
                using namespace opendlv::proxy;
                PedalPositionReading msg;
                msg.percent(currentSpeed);
                motorBroadcast->send(msg);
                break;
            }
            case 6: {
                if (currentSpeed > 0.0) {
                    currentSpeed -= 0.05;
                }            
                using namespace opendlv::proxy;
                PedalPositionReading msg;
                msg.percent(currentSpeed);
                motorBroadcast->send(msg);
                break;
            }
            case 7: {
                currentSteering += 0.05;
                using namespace opendlv::proxy;
                GroundSteeringReading msg;
                msg.steeringAngle(currentSteering);
                motorBroadcast->send(msg);
                break;
            }
            case 8: {
                currentSteering -= 0.05;
                using namespace opendlv::proxy;
                GroundSteeringReading msg;
                msg.steeringAngle(currentSteering);
                motorBroadcast->send(msg);
                break;
            }
            case 9: {
                InternalAnnouncePresence msg;
                internalBroadcast->send(msg);
                break;
            }
            default: {
                return 0;
            }
        }
    }
}
