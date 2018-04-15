#include <iostream>
#include <map>
#include <fstream>
#include <string>

#include "v2v/v2v.hpp"

/**
 * Main file of the V2V microservice. Here we initialize the V2VService object with provided parameters and supply the
 * user with a simple direct interaction command line interface.
 */

using namespace std;
int main(int argc, char** argv) {

    // Check that both IP address and groupid for the service has been provided.
    if (argc < 3) {
        cout << "You need to provide <ip-address> and <group ID>" << endl;
        exit(1);
    }
    /*
     * argv[1] = IP
     * argv[2] = Group ID
     */
    shared_ptr<V2VService> v2vService = make_shared<V2VService>(argv[1], argv[2]);

    // Messages to test
    while (true) {
        unsigned int choice;
        cout << "Which message would you like to send?" << endl;
        cout << "(1) AnnouncePresence" << endl;
        cout << "(2) FollowRequest" << endl;
        cout << "(3) FollowResponse" << endl;
        cout << "(4) StopFollow" << endl;
        cout << "(5) LeaderStatus" << endl;
        cout << "(6) FollowerStatus" << endl;
        cout << "### UTILITY AND TESTING ###" << endl;
        cout << "(7) V2VService Health check" << endl;
        cout << "(#) Nothing, just quit." << endl;
        cout << ">> ";
        cin >> choice;

        switch (choice) {
            case 1: {
                v2vService->announcePresence();
                break;
            }
            case 2: {
                if (ipMap.size() < 1) {
                    cout << "There are no IPs to choose from, wait for someone to announce themselves!" << endl;
                    break;
                }

                map<string, string> ipMap = v2vService->getMapOfIps();
                std::string groupId = "";

                cout << "Which vehicle would you like to follow?" << endl;
                for(std::map<std::string, std::string>::iterator it = ipMap.begin(); it != ipMap.end(); ++it) {
                    cout << it->first << " " << it->second << endl;
                }
                cin >> groupId;

                cout << "You chose: " << groupId << endl;
                v2vService->followRequest(ipMap[groupId]);
                
                break;
            }
            case 3: {
                v2vService->followResponse();
                break;
            }
            case 4: {
                v2vService->stopFollow();
                break;
            }
            case 5: {
                v2vService->leaderStatus(0, 0, 0);
                break;
            }
            case 6: {
                v2vService->followerStatus();
                break;
            }
            case 7: {
                v2vService->healthCheck();
                break;
            }
            default: {
                return 0;
            }
        } // end switch
    } // end while
} // end main
