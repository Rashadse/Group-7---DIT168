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
            cout << "[RC SIM] ";
            
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
                default:
                    cout << "Got a message I could not understand!" << endl;
                    break;
            }
        }
    );


    while (true) {
        unsigned int choice;
        cout << "Which message would you like to send?" << endl;
        cout << "(1) InternalFollow" << endl;
        cout << "(2) InternalStopFollow" << endl;
        cout << "(3) InternalGetAllGroups" << endl;
        cout << "(4) InternalEmergencyBrake" << endl;
        cout << "(#) Nothing, just quit." << endl;
        cout << ">> ";
        cin >> choice;
        
     
        switch (choice) {
            case 1: {
                InternalFollowRequest msg;
                msg.groupid("6");
                internalBroadcast->send(msg);
                break;
            }
            case 2: {
                InternalStopFollow msg;
                msg.groupid("6");   
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
            default: return 0;
        }
    }
}