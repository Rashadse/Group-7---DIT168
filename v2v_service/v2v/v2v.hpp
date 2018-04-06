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
#include <set>
#include <map>
#include <string>

static const std::string CAR_IP = "192.168.43.171";
static const std::string GROUP_ID = "7";

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
    V2VService();

    void announcePresence();
    void followRequest(std::string vehicleIp);
    void followResponse();
    void stopFollow();
    void leaderStatus(uint8_t speed, uint8_t steeringAngle, uint8_t distanceTraveled);
    void followerStatus(uint8_t speed, uint8_t steeringAngle, uint8_t distanceFront, uint8_t distanceTraveled);

    std::set<std::string> getAnnouncedIps();
    //getting the map
    std::map<std::string, std::string> getMapOfIps();
    //end of map

private:
    std::set<std::string> announcedIps;
    //creating a map
    std::map<std::string, std::string> mapOfIps;
    //end of map

    std::string leaderIp;
    std::string followerIp;

    std::shared_ptr<cluon::OD4Session>  broadcast;
    std::shared_ptr<cluon::UDPReceiver> incoming;
    std::shared_ptr<cluon::UDPSender>   toLeader;
    std::shared_ptr<cluon::UDPSender>   toFollower;

    static uint32_t getTime();
    static std::pair<int16_t, std::string> extract(std::string data);
    template <class T>
    static std::string encode(T msg);
    template <class T>
    static T decode(std::string data);
};

#endif // V2V_PROTOCOL_H
