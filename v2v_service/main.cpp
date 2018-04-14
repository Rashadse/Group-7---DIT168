#include <iostream>
#include <map>
#include <fstream>
#include <string>

#include "v2v/v2v.hpp"


using namespace std;
int main(int /*argc*/, char** /*argv*/) {
    shared_ptr<V2VService> v2vService = make_shared<V2VService>("192.168.43.212", "7");

    while (true) {
        unsigned int choice;
        cout << "Which message would you like to send?" << endl;
        cout << "(1) AnnouncePresence" << endl;
        cout << "(2) FollowRequest" << endl;
        cout << "(3) FollowResponse" << endl;
        cout << "(4) StopFollow" << endl;
        cout << "(5) LeaderStatus" << endl;
        cout << "(6) FollowerStatus" << endl;
        cout << "UTILITY AND TESTING" << endl;
        cout << "(7) List all announced IPs" << endl;
        cout << "(8) V2VService Health check" << endl;
        cout << "(#) Nothing, just quit." << endl;
        cout << ">> ";
        cin >> choice;
        
        
        map<string, string> ipMap;
        std::string groupId = "";
        switch (choice) {
            case 1: v2vService->announcePresence(); break;
            case 2:
                ipMap = v2vService->getMapOfIps();
                if (ipMap.size() < 1) {
                    cout << "There are no IPs to choose from, wait for someone to announce themselves!" << endl;
                    break;
                }
                
                cout << "Which vehicle would you like to follow?" << endl;
                for(std::map<std::string, std::string>::iterator it = ipMap.begin(); it != ipMap.end(); ++it) {
                    cout << it->first << " " << it->second << endl;
                }
                cin >> groupId;
                cout << "You chose: " << groupId << endl;
                v2vService->followRequest(ipMap[groupId]);
                
                break;
            case 3: v2vService->followResponse(); break;
            case 4: v2vService->stopFollow(); break;
            case 5: v2vService->leaderStatus(0, 0, 0); break;
            case 6: v2vService->followerStatus(); break;
            case 7:
                ipMap = v2vService->getMapOfIps();
                for(std::map<std::string, std::string>::iterator it = ipMap.begin(); it != ipMap.end(); ++it) {
                    cout << it->first << " " << it->second << endl;
                }
                break;
            case 8:
                v2vService->healthCheck();
                break;
            default: return 0;
        }    
    }
}
