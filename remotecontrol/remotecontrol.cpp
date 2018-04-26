#include <iostream>
#include "remotecontrol.hpp"
using namespace std;

int main() {	

		std::shared_ptr<cluon::OD4Session> internalChannel = std::make_shared<cluon::OD4Session>(INTERNAL_CHANNEL,
        	[](cluon::data::Envelope &&envelope) noexcept {	

               std::cout << "internal channel ";
                switch (envelope.dataType()) {

                     case INTERNAL_ANNOUNCE_PRESENCE: {
 		InternalAnnouncePresence msg = cluon::extractMessage<InternalAnnouncePresence>(std::move(envelope));
		 cout << "[RC INTERNAL] Internal announced presence" << endl;

                    break;
                }

                     case INTERNAL_FOLLOW_RESPONSE: {
                    InternalFollowResponse msg = cluon::extractMessage<InternalFollowResponse>(std::move(envelope));
                    
                    cout << "[RC INTERNAL] Now following group: " << msg.groupid() << " status: " << msg.status() << endl;
                    break;
                }
                case INTERNAL_STOP_FOLLOW_RESPONSE: {
                    InternalStopFollowResponse msg = cluon::extractMessage<InternalStopFollowResponse>(std::move(envelope));
                    
                    cout << "[RC INTERNAL] Stopped following group: " << msg.groupid() << endl;
                    break;
                }
                case INTERNAL_GET_ALL_GROUPS_RESPONSE: {
                    InternalGetAllGroupsResponse msg = cluon::extractMessage<InternalGetAllGroupsResponse>(std::move(envelope));
                    
                    cout << "[RC INTERNAL] Got group: " << msg.groupid() << endl;
                    break;
                }
                case INTERNAL_EMERGENCY_BRAKE: {
                    InternalEmergencyBrake msg = cluon::extractMessage<InternalEmergencyBrake>(std::move(envelope));
                    
                    cout << "[RC INTERNAL] Emergency brake!" << endl;
                    break;
                }
                default: {
                    break;
                }
	}
});
	
   
   while (true) {


        unsigned int choice;
        cout << "Which message would you like to send?" << endl;
        cout << "(1) InternalFollow" << endl;
        cout << "(2) InternalStopFollow" << endl;
        cout << "(3) InternalGetAllGroups" << endl;
        cout << "(4) InternalEmergencyBrake" << endl;
        cout << "(5) Announce your presence!" << endl;
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
                internalChannel->send(msg);
                break;
            }
            case 2: {
                InternalStopFollow msg;
                std::string groupId = "";
                
                cout << "Which vehicle would you like to stop following?" << endl;
                cin >> groupId;
                
                msg.groupid(groupId);
                internalChannel->send(msg);
                break;
            }
            case 3: { 
                InternalGetAllGroupsRequest msg;
                internalChannel->send(msg);                   
                break;
            }
            case 4: {
                InternalEmergencyBrake msg;
                internalChannel->send(msg);
                break;
            }

            case 5: {
                InternalAnnouncePresence msg;
                internalChannel->send(msg);
                break;
            }
            default: {
                return 0;
            }           
	}
    }
}




