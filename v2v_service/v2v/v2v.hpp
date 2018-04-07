#ifndef V2V_PROTOCOL_H
#define V2V_PROTOCOL_H

#include <iomanip>
#include <unistd.h>
#include <sys/time.h>
#include "cluon/OD4Session.hpp"
#include "cluon/UDPSender.hpp"
#include "cluon/UDPReceiver.hpp"
#include "cluon/Envelope.hpp"
#include "messages.hpp"
#include <map>
#include <string>
#include <pthread.h>

static const int BROADCAST_CHANNEL = 200;
static const int DEFAULT_PORT = 50001;

static const int ANNOUNCE_PRESENCE = 1001;
static const int FOLLOW_REQUEST = 1002;
static const int FOLLOW_RESPONSE = 1003;
static const int STOP_FOLLOW = 1004;
static const int LEADER_STATUS = 2001;
static const int FOLLOWER_STATUS = 3001;


class V2VService {
public:
    V2VService(std::string ip, std::string groupId);

    void announcePresence();
    void followRequest(std::string vehicleIp);
    void followResponse();
    void stopFollow();

    pthread_t leaderStatusThread;
    pthread_t followerStatusThread;
    
    uint32_t lastFollowerUpdate;
    uint32_t lastLeaderUpdate;

    void startReportingToFollower();
    void leaderStatus(uint8_t speed, uint8_t steeringAngle, uint8_t distanceTraveled);

    void startReportingToLeader();
    void followerStatus(uint8_t speed, uint8_t steeringAngle, uint8_t distanceFront, uint8_t distanceTraveled);

    std::map<std::string, std::string> getMapOfIps();
    
    static uint32_t getTime();

private:
    std::map<std::string, std::string> mapOfIps;

    std::string myIp;
    std::string myGroupId;
    
    std::string leaderIp;
    std::string followerIp;

    std::shared_ptr<cluon::OD4Session>  broadcast;
    std::shared_ptr<cluon::UDPReceiver> incoming;
    std::shared_ptr<cluon::UDPSender>   toLeader;
    std::shared_ptr<cluon::UDPSender>   toFollower;

    static std::pair<int16_t, std::string> extract(std::string data);
    template <class T>
    static std::string encode(T msg);
    template <class T>
    static T decode(std::string data);
};

#endif // V2V_PROTOCOL_H
