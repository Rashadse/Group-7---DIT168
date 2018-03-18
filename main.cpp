#include <iostream>

#include "v2v.hpp"


using namespace std;
int main(int /*argc*/, char** /*argv*/) {
    shared_ptr<V2VService> v2vService = make_shared<V2VService>();

    while (true) {
        int choice;
        cout << "Which message would you like to send?" << std::endl;
        cout << "(1) AnnouncePresence" << std::endl;
        cout << "(2) FollowRequest" << std::endl;
        cout << "(3) FollowResponse" << std::endl;
        cout << "(4) StopFollow" << std::endl;
        cout << "(5) LeaderStatus" << std::endl;
        cout << "(6) FollowerStatus" << std::endl;
        cout << "(#) Nothing, just quit." << std::endl;
        cout << ">> ";
        cin >> choice;

        switch (choice) {
            case 1: v2vService->announcePresence(); break;
            case 2: v2vService->followRequest(CAR_IP); break;
            case 3: v2vService->followResponse(); break;
            case 4: v2vService->stopFollow(CAR_IP); break;
            case 5: v2vService->leaderStatus(50, 0, 100); break;
            case 6: v2vService->followerStatus(50, 0, 10, 100); break;
            default: return 0;
        }
    }
}
