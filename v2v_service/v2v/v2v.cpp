#include <iostream>
#include "v2v.hpp"
#include <map>

/**
 * Implementation of the V2VService class as declared in v2v.hpp
 */

/**
 * Constructor for the V2V service class.
 *
 * @param ip - IP address of the car running the service
 * @param groupId - ID of the car running the service
 */
V2VService::V2VService(std::string ip, std::string groupId) {
    followerIp = "";
    leaderIp = "";
    myIp = ip;
    myGroupId = groupId;
    currentCarStatus.speed = 0;
    currentCarStatus.steeringAngle = 0;
    
    /*
     * The broadcast field contains a reference to the broadcast channel which is an OD4Session. This channel is where
     * AnnouncePresence messages will be received.
     */
    broadcast = std::make_shared<cluon::OD4Session>(
        BROADCAST_CHANNEL,
        [this](cluon::data::Envelope &&envelope) noexcept {

            switch (envelope.dataType()) {
                case ANNOUNCE_PRESENCE: {
                    AnnouncePresence ap = cluon::extractMessage<AnnouncePresence>(std::move(envelope));
                    std::cout << "[BROADCAST] received 'AnnouncePresence' from '"
                              << ap.vehicleIp() << "', GroupID '"
                              << ap.groupId() << "'!" << std::endl;

                    // Filter out yourself from announcement
                    if (ap.groupId() != myGroupId) {
                        mapOfIps.insert(std::make_pair(ap.groupId(), ap.vehicleIp()));
                        mapOfIds[ap.vehicleIp()] = ap.groupId(); // Map for being able to get a groupid
                    }                                            // from an IP as well.

                    break;
                }
                default: {
                    break;
                }
            } // end switch
        } // end lambda
    ); // end broadcast declaration

    /*
     * This OD4 session takes care of car internal communication over the STS (service to service) protocol.
     */
    internalBroadCast = std::make_shared<cluon::OD4Session>(
        INTERNAL_BROADCAST_CHANNEL,
        [this](cluon::data::Envelope &&envelope) noexcept {

            switch (envelope.dataType()) {
                case INTERNAL_ANNOUNCE_PRESENCE: {
                    announcePresence();

                    break;
                }
                case INTERNAL_FOLLOW_REQUEST: {
                    InternalFollowRequest msg = cluon::extractMessage<InternalFollowRequest>(std::move(envelope));

                    // In case we already have a leader, we return an internal follow response with
                    // a negative result code right away.
                    if (leaderIp.empty()){
                        followRequest(mapOfIps[msg.groupid()]);
                    } else {
                        InternalFollowResponse msg;
                        msg.groupid(msg.groupid());
                        msg.status(0);
                        internalBroadCast->send(msg);
                    }

                    break;
                }
                case INTERNAL_STOP_FOLLOW_REQUEST: {
                    InternalStopFollow msg = cluon::extractMessage<InternalStopFollow>(std::move(envelope));

                    stopFollow();

                    InternalStopFollowResponse retmsg;
                    retmsg.groupid(msg.groupid());
                    internalBroadCast->send(retmsg);

                    break;
                }
                case INTERNAL_GET_ALL_GROUPS_REQUEST: {
                    InternalGetAllGroupsRequest msg = cluon::extractMessage<InternalGetAllGroupsRequest>(std::move(envelope));

                    // Iterate over the internal map of IPs and send the groupids back to the requester.
                    std::map<std::string, std::string> ipMap = getMapOfIps();
                    for(std::map<std::string, std::string>::iterator it = ipMap.begin(); it != ipMap.end(); ++it) {
                        InternalGetAllGroupsResponse msg;
                        msg.groupid(it->first);
                        internalBroadCast->send(msg);
                    }
                    break;
                }
                case INTERNAL_EMERGENCY_BRAKE: {
                    InternalEmergencyBrake msg = cluon::extractMessage<InternalEmergencyBrake>(std::move(envelope));
                    // Terminate communication with any other vehicle.
                    stopFollow();

                    std::cout << "received '" << msg.LongName() << std::endl;
                    break;
                }
                default: {
                    break;
                }
            } // end switch
        } // end lambda
    ); // end internalBroadcast declaration

    /*
     * We use the motorBroadcast to keep the V2V service updated on the car's current status (in terms of speed and
     * steering angle) to accurately be able to send the latest car status in the LeaderStatus message.
     */
    motorBroadcast = std::make_shared<cluon::OD4Session>(
        MOTOR_BROADCAST_CHANNEL,
        [this](cluon::data::Envelope &&envelope) noexcept {
            
            using namespace opendlv::proxy;
            switch (envelope.dataType()) {
                case PEDAL_POSITION_READING: {
                    PedalPositionReading msg = cluon::extractMessage<PedalPositionReading>(std::move(envelope));
                    currentCarStatus.speed = msg.percent();
                    break;
                }
                case GROUND_STEERING_READING: {
                    GroundSteeringReading msg = cluon::extractMessage<GroundSteeringReading>(std::move(envelope));
                    currentCarStatus.steeringAngle = msg.steeringAngle();

                    std::cout << "New steering: " << msg.steeringAngle() << std::endl;

                    break;
                }
                case DISTANCE_READING: {
                    DistanceReading msg = cluon::extractMessage<DistanceReading>(std::move(envelope));
                    sensorRange[index] = msg.distance() * 100;
                    index++;
                    if (index ==  5) {
                       index = 0;
                    }

                    float sum = 0;
                    for (int i = 0; i <= 4; i++ ) {
                        sum += sensorRange[i];
                    }
                    float average = sum / 5;
                    //std::cout << average << std::endl;

                    if(average <= 40)
                    {
                    	stopCar();
                        InternalEmergencyBrake stopped; 
                        internalBroadCast->send(stopped);
                    }
                    break;
                }
                default: {
                    break;
                }
            } // end switch
        } // end lambda
    ); // end motorBroadcast declaration

    /*
     * Each car declares an incoming UDPReceiver for messages directed at them specifically. This is where messages
     * such as FollowRequest, FollowResponse, StopFollow, etc. are received.
     */
    incoming = std::make_shared<cluon::UDPReceiver>(
        "0.0.0.0",
        DEFAULT_PORT,
        [this](std::string &&data, std::string &&sender, std::chrono::system_clock::time_point /*&&ts*/) noexcept {
            std::pair<int16_t, std::string> msg = extract(data);

            std::string senderIp = sender.substr(0, sender.find(":"));

            switch (msg.first) {
                case FOLLOW_REQUEST: {
                    FollowRequest followRequest = decode<FollowRequest>(msg.second);
                    std::cout << "[INCOMING] received '" << followRequest.LongName()
                               << "' from '" << senderIp << "'!" << std::endl;
                               
                    // Propagate the message to internal for visualization
                    internalBroadCast->send(followRequest);

                    // After receiving a FollowRequest, check first if there is currently no car already following.
                    if (followerIp.empty()) {
                        followerIp = senderIp; // If no, add the requester to known follower slot and establish a
                        // sending channel.
                        toFollower = std::make_shared<cluon::UDPSender>(followerIp, DEFAULT_PORT);
                        followResponse();

                        startReportingToFollower();
                    }
                    break;
                }
                case FOLLOW_RESPONSE: {
                    FollowResponse followResponse = decode<FollowResponse>(msg.second);
                    std::cout << "[INCOMING] received '" << followResponse.LongName()
                              << "' from '" << senderIp << "'!" << std::endl;
                              
                    internalBroadCast->send(followResponse);

                    // Makes sure we do not accept any rogue responses.
                    if (senderIp == leaderIp) {
                        isLeaderMoving = false; // Until we receive the first leader status, we assume the leader is standstill.
                        
                        startReportingToLeader();
                        
                        startFollowing();

                        InternalFollowResponse msg;
                        msg.groupid(mapOfIds[senderIp]);
                        msg.status(1);
                        internalBroadCast->send(msg);
                    }
                    break;
                }
                case STOP_FOLLOW: {
                    StopFollow stopFollow = decode<StopFollow>(msg.second);
                    std::cout << "[INCOMING] received '" << stopFollow.LongName()
                              << "' from '" << senderIp << "'!" << std::endl;
                              
                    internalBroadCast->send(stopFollow);

                    // Clear either follower or leader slot, depending on current role.
                    if (senderIp == followerIp) {
                        followerIp = "";
                        toFollower.reset();
                        lastFollowerUpdate = 0;
                    }
                    else if (senderIp == leaderIp) {
                        leaderIp = "";
                        toLeader.reset();
                        lastLeaderUpdate = 0;
                        
                        // If it was the leader who sent the stop follow, we should also stop our car.
                        stopCar();
                    }
                    
                    break;
                }
                case FOLLOWER_STATUS: {
                    FollowerStatus followerStatus = decode<FollowerStatus>(msg.second);
                    std::cout << "[INCOMING] received '" << followerStatus.LongName()
                              << "' from '" << senderIp << "'!" << std::endl;
                              
                    internalBroadCast->send(followerStatus);

                    // If we have a follower, update the last received time for follower status.
                    if (!followerIp.empty()) {
                        lastFollowerUpdate = getTime();
                    }

                    break;
                }
                case LEADER_STATUS: {
                    LeaderStatus leaderStatus = decode<LeaderStatus>(msg.second);
                    std::cout << "[INCOMING] received '" << leaderStatus.LongName() <<
                                 " - New speed = " << leaderStatus.speed() <<
                                 " - New steering = " << leaderStatus.steeringAngle() << std::endl;
                                 
                    internalBroadCast->send(leaderStatus);

                    // Only process the messages from the leader.
                    if (senderIp == leaderIp) {
                        processLeaderStatus(leaderStatus);
                    }
                    break;
                }
                default: {
                    break;
                }
            } // end switch
        } // end lambda
    ); // end incoming declaration
} // end constructor

/**
 * This function sends an AnnouncePresence (id = 1001) message on the broadcast channel. It will contain information
 * about the sending vehicle, including: IP, port and the group identifier.
 */
void V2VService::announcePresence() {
    AnnouncePresence announcePresence;
    announcePresence.vehicleIp(myIp);
    announcePresence.groupId(myGroupId);
    broadcast->send(announcePresence);
}

/**
 * This function sends a FollowRequest (id = 1002) message to the IP address specified by the parameter vehicleIp. And
 * sets the current leaderIp field of the sending vehicle to that of the target of the request.
 *
 * @param vehicleIp - IP of the target for the FollowRequest
 */
void V2VService::followRequest(std::string vehicleIp) {
    if (!leaderIp.empty()) return;
    leaderIp = vehicleIp;
    toLeader = std::make_shared<cluon::UDPSender>(leaderIp, DEFAULT_PORT);
    FollowRequest followRequest;
    toLeader->send(encode(followRequest));
    
    internalBroadCast->send(followRequest);
}

/**
 * This function send a FollowResponse (id = 1003) message and is sent in response to a FollowRequest (id = 1002).
 * This message will contain the NTP server IP for time synchronization between the target and the senderIp.
 */
void V2VService::followResponse() {
    if (followerIp.empty()) return;
    FollowResponse followResponse;
    toFollower->send(encode(followResponse));
    
    internalBroadCast->send(followResponse);
}

/**
 * This function sends a StopFollow (id = 1004) request on the ip address of the parameter vehicleIp. If the IP address
 * is neither that of the follower nor the leader, this function ends without sending the request message.
 *
 * @param vehicleIp - IP of the target for the request
 */
void V2VService::stopFollow() {
    // Clear comm channels
    StopFollow stopFollow;
    if (leaderIp != "") {
    	toLeader->send(encode(stopFollow));
    	leaderIp = "";
     	toLeader.reset();
     	
     	// If we want to stop following the leader, we need to stop our car.
     	stopCar();
    }
    if (followerIp != "") {
	    toFollower->send(encode(stopFollow));
     	followerIp = "";
     	toFollower.reset();
    }
    
    // Clear timestamps
    lastFollowerUpdate = 0;
    lastLeaderUpdate = 0;
    
    isLeaderMoving = false;
    
    internalBroadCast->send(stopFollow);
}

/**
 * This function is designed to be the target of a pthread starting. It will take care of sending FollowerStatus
 * messages to the leading car as well as check that the leading car has been sending regular updates.
 *
 * @param v2v - the v2v service object reference from which this thread function will both take and get information from
 * @return N/A
 */
void *sendFollowerStatuses(void *v2v) {
    V2VService *v2vservice;
    v2vservice = (V2VService *)v2v;
    std::cout << "Send follower status thread started!" << std::endl;

    using namespace std::chrono_literals;
    while (!v2vservice->leaderIp.empty()) {        

        // Since leader updates are more frequent than follower statuses,
        // we disconnect after only two seconds of radio silence.
        if ((v2vservice->getTime() - v2vservice->lastLeaderUpdate) > 1000) {
            v2vservice->stopFollow();
            break;
        }

        v2vservice->followerStatus();
        std::this_thread::sleep_for(500ms);
    }

    pthread_exit(NULL);
}

/**
 * This function starts the thread that will take care of sending statuses to the leading vehicle.
 */
void V2VService::startReportingToLeader() {
    // Get time before reporting was started to break connection in case no updates are received for over one second
    lastLeaderUpdate = getTime();

    int status;
    pthread_t threadId;
    status = pthread_create(&threadId, NULL, sendFollowerStatuses, (void *)this);
    
    // pthread_create returns 1 if an error occured.
    if (status) {
        std::cout << "Error creating update leader thread" << std::endl;
    }
}

/**
 * This function sends a FollowerStatus (id = 3001) message on the leader channel.
 */
void V2VService::followerStatus() {
    if (leaderIp.empty()) return;
    FollowerStatus followerStatus;
    toLeader->send(encode(followerStatus));
    
    internalBroadCast->send(followerStatus);
}

/**
 * This function is designed to be the target of a pthread starting. It will take care of sending FollowerStatus
 * messages to the leading car as well as check that the leading car has been sending regular updates.
 *
 * @param v2v - the v2v service object reference from which this thread function will both take and get information from
 * @return N/A
 */
void *executeLeaderUpdates(void *v2v) {
    V2VService *v2vservice;
    v2vservice = (V2VService *)v2v;

    std::queue<std::pair<uint64_t, LeaderStatus>> *updateQueue;
    updateQueue = v2vservice->getLeaderUpdates(); // Get reference to leader status update queue
    std::cout << "Execute leader updates thread started!" << std::endl;

    std::pair<uint64_t, LeaderStatus> currentUpdate;
    LeaderStatus leaderStatus;


    using namespace std::chrono_literals;
    while (!v2vservice->leaderIp.empty()) {        

        // If leader car is moving and the update queue is not empty...
        if (v2vservice->isLeaderMoving && !updateQueue->empty()) {
            /*
             * If leader is moving, pop the update queue and wait for the set time before actuating.
             */
            currentUpdate = updateQueue->front();
            updateQueue->pop();
            std::chrono::milliseconds sleepTime(currentUpdate.first);
            std::cout << "Sleeping for before executing queued update..." << std::endl;
            std::this_thread::sleep_for(sleepTime);

            std::cout << "Executing queued leader status!" << std::endl;
            leaderStatus = currentUpdate.second;
            v2vservice->sendSpeed(leaderStatus.speed());
            v2vservice->sendSteering(leaderStatus.steeringAngle());
            
        } else if (!v2vservice->isLeaderMoving) {
            /*
             * Necessary for a special case where during the above sleep the leader stops moving and we execute the
             * command towards the motor anyway. We should then fall into here and stop the car shortly thereafter.
             */
            v2vservice->stopCar();
            std::this_thread::sleep_for(50ms);
        }
    }

    pthread_exit(NULL);
}

/**
 * This function starts the thread that will take care of sending statuses to the leading vehicle.
 */
void V2VService::startFollowing() {
    lastLeaderUpdate = getTime();

    // Empty the old queue since new following has been initialised.
    std::queue<std::pair<uint64_t, LeaderStatus>> newQueue;

    /*
     * This will prefill the leader status queue with updates to go the first 1 meter straight,
     * right now takes about 3 seconds in total at 15% power and (naturally) 0.0 steering.
     *
     * 125ms * 24 = 3000ms = 3 seconds.
     */
    std::cout << "Starting to pre fill update queue" << std::endl;
    uint64_t time = getTime();
    for (int i = 0; i < 7; i++) {
        std::pair<uint64_t, LeaderStatus> initialUpdate;
        LeaderStatus leaderStatus;
        leaderStatus.speed(0.15);
        leaderStatus.steeringAngle(0.0);

        initialUpdate.first = 125; // Standard delay
        initialUpdate.second = leaderStatus;

        newQueue.push(initialUpdate);
    }
    std::cout << "Pre fill took " << (getTime() - time) << "ms" << std::endl;
    /*
     * Perform swap here to avoid problem where leader statuses get filled simultaneously, that's because the sequence
     * looks like this:
     *
     * Other car                    Our car
     *     |  <-- Follow Request ----  |
     *     |  --- Follow Response -->  |
     * < Starts sending >     < Start listening >
     *     |                           |
     *
     * If we do not swap the queues here, we could get interfering leader statuses during the pre fill which will be
     * erroneously inserted in the middle of the "pre fill" updates, meaning the car could suddenly turn slightly
     * during ramp-up. Swapping here makes sure that even if we received some updates during pre fill, they will be
     * ignored. Trade off, but should not mean losing more than 1 update in the absolute worst case.
     */
    std::swap(leaderUpdates, newQueue);

    int status;
    pthread_t threadId;
    status = pthread_create(&threadId, NULL, executeLeaderUpdates, (void *)this);
    
    // pthread_create returns 1 if an error occured.
    if (status) {
        std::cout << "Error creating update leader thread" << std::endl;
    }
}

/**
 * This function is designed to be the target of a pthread starting. It will take care of sending LeaderStatus messages
 * to the following car as well as check that the following car has been sending regular updates.
 *
 * @param v2v - the v2v service object reference from which this thread function will both take and get information from
 * @return N/A
 */
void *sendLeaderStatuses(void *v2v) {
    V2VService *v2vservice;
    v2vservice = (V2VService *)v2v;
    std::cout << "Update follower thread started!" << std::endl;
    
    using namespace std::chrono_literals;
    while (!v2vservice->followerIp.empty()) {   // Report as long as we have a follower
    
        // If no update has been received from follower for more than two seconds, disconnect
        if ((v2vservice->getTime() - v2vservice->lastFollowerUpdate) > 2000) {
            v2vservice->stopFollow();
            break;
        }
        
        // Get sensor data
        CarStatus *currentCarStatus = v2vservice->getCurrentCarStatus();
        
        // Send sensor data
        v2vservice->leaderStatus(
            currentCarStatus->speed,
            currentCarStatus->steeringAngle
        );
        // Message frequency according to protocol.
        std::this_thread::sleep_for(125ms);
    }
    
    pthread_exit(NULL);
}

/**
 * This function starts the thread that will take care of sending current car statuses to the following vehicle.
 */
void V2VService::startReportingToFollower() {
    // Get time before reporting was started to break connection in case no updates are received for over two seconds
    lastFollowerUpdate = getTime();

    int status;
    pthread_t threadId;
    status = pthread_create(&threadId, NULL, sendLeaderStatuses, (void *)this);
    
    // pthread_create returns 1 if an error occured.
    if (status) {
        std::cout << "Error creating update follower thread" << std::endl;
    }
}

/**
 * This function processes an incoming LeaderStatus message and actuates towards the motor and steering based
 * on the input data.
 *
 * @param leaderStatusUpdate - latest status update from leading vehicle to process
 */
void V2VService::processLeaderStatus(LeaderStatus leaderStatusUpdate) {
    float speed = leaderStatusUpdate.speed();
    
    if (speed == 0) { // Maybe add a higher lower bound since car does not move until 15~ percent?
        /* 
        No point in logging (inserting into queue) a speed of 0 since any included steering will 
        have no effect to movement. 
        */
        isLeaderMoving = false; // This is to make sure we only move then the leader does.
        
        sendSpeed(speed);
    } else {
        isLeaderMoving = true; // This is to make sure we only move when the leader does.
    
        std::pair<uint64_t, LeaderStatus> update;
    
        if (lastLeaderUpdate == 0) { // If the last leader update was not registered yet for 
                                     // some reason, use the default time delay.
            update.first = 125;
        } else {
            // Actual time between updates
            //update.first = getTime() - lastLeaderUpdate;
            update.first = 125;
        }
        
        update.second = leaderStatusUpdate;
        leaderUpdates.push(update);
    }
    
    lastLeaderUpdate = getTime(); 
}

/**
 * Sends actuating messages to motor channel to stop the car.
 */
void V2VService::stopCar() {

    if (currentCarStatus.speed > 0) {
        sendSteering(0.0);
        sendSpeed(0.0);
    }
}

void V2VService::sendSteering(float steering) {
    opendlv::proxy::GroundSteeringReading steeringMsg;

    if (steering == 0) {
        steeringMsg.steeringAngle(steering + (steeringOffset));
    } else {
        steeringMsg.steeringAngle(steering);
    }
    motorBroadcast->send(steeringMsg);
}

void V2VService::sendSpeed(float speed) {
    opendlv::proxy::PedalPositionReading speedMsg;

    // Offset only applies for speeds > 0.
    if (speed == 0) {
        speedMsg.percent(speed);
        motorBroadcast->send(speedMsg);
    } else {
        speedMsg.percent(speed + (speedOffset));
        motorBroadcast->send(speedMsg);
    }
}

/**
 * This function sends a LeaderStatus (id = 2001) message on the follower channel.
 *
 * @param speed - current pedal position
 * @param steeringAngle - current steering angle
 * @param distanceTraveled - distance traveled since last sending a leader status message
 */
void V2VService::leaderStatus(float speed, float steeringAngle) {
    uint8_t distanceTraveled = 0;
    if (speed <= 0.15 && speed >= 0.13){
        distanceTraveled = 7;
    }

    if (followerIp.empty()) return;
    LeaderStatus leaderStatus;
    leaderStatus.timestamp(getTime());
    leaderStatus.speed(speed);
    leaderStatus.steeringAngle(steeringAngle);
    leaderStatus.distanceTraveled(distanceTraveled);
    toFollower->send(encode(leaderStatus));
    
    internalBroadCast->send(leaderStatus);
}

/**
 * This functions gets a map containing IP addresses of cars & their groupIds as they announced presence in the network.
 *
 * @return mapOfIps - a map containing the IP addresses and the groupIds of all cars that have announced their presence.
 */
std::map<std::string, std::string> V2VService::getMapOfIps() {
    return mapOfIps;
}

/**
 * Getter for the currentCarStatus field.
 *
 * @return currentCarStatus - the car's current status in terms of speed, steering etc.
 */
CarStatus *V2VService::getCurrentCarStatus() {
    return &currentCarStatus;
}

std::queue<std::pair<uint64_t, LeaderStatus>> *V2VService::getLeaderUpdates() {
    return &leaderUpdates;
}

/**
 * Setter for the currentCarStatus field.
 *
 * @param newCarStatus - new car status to override current car status
 * @return newCurrentCarStatus - the new status of the car
 */
CarStatus *V2VService::setCurrentCarStatus(struct CarStatus *newCarStatus) {
    currentCarStatus.speed = newCarStatus->speed;
    currentCarStatus.steeringAngle = newCarStatus->steeringAngle;
    return &currentCarStatus;
}

/**
 * A simple printout containing information about the V2VService object state.
 */
void V2VService::healthCheck() {
    std::map<std::string, std::string> ipMap = getMapOfIps();
    CarStatus *status = getCurrentCarStatus();
    std::cout << "V2VService health check" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "GroupID : " << myGroupId << " IP-address : " << myIp << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Current Time (s)  : " << getTime() << std::endl;
    std::cout << "Current speed (%) : " << status->speed << std::endl;
    std::cout << "Current angle (%) : " << status->steeringAngle << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Follower          : " << followerIp << std::endl;
    std::cout << "Leader            : " << leaderIp << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Announced         : " << followerIp << std::endl;
    for(std::map<std::string, std::string>::iterator it = ipMap.begin(); it != ipMap.end(); ++it) {
        std::cout << "    Group " << it->first << " - IP " << it->second << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
}

/**
 * Gets the current time.
 *
 * @return current time in seconds
 */
uint64_t V2VService::getTime() {
    using namespace std::chrono;
    
    milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    );
    return (uint64_t) ms.count();
}

/**
 * The extraction function is used to extract the message ID and message data into a pair.
 *
 * @param data - message data to extract header and data from
 * @return pair consisting of the message ID (extracted from the header) and the message data
 */
std::pair<int16_t, std::string> V2VService::extract(std::string data) {
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
std::string V2VService::encode(T msg) {
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
T V2VService::decode(std::string data) {
    std::stringstream buff(data);
    cluon::FromProtoVisitor v;
    v.decodeFrom(buff);
    T tmp = T();
    tmp.accept(v);
    return tmp;
}
