//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Boss.h"

Define_Module(Boss);

void Boss::initialize()
{
    //initialize parameters
    arrival_mean = par("arrival_mean");
    service_mean = par("service_mean");
    arrival_distribution = par("arrival_distribution");
    service_distribution = par("service_distribution");

    timer_ = new cMessage("timer");

    //first arrival
    scheduleNextArrival();
}

void Boss::handleMessage(cMessage *msg)
{
    //generate new BOSS
    generateNewOpponent();

    //schedule a new opponent arrival
    scheduleNextArrival();
}

/**
 * Schedule a new opponent arrival with random (extracted from an exponential distribution) or constant time
 */
void Boss::scheduleNextArrival(){
    simtime_t arrival_time;

    //to handle the degeneracy test
    if (arrival_mean != 0 && service_mean != 0){ //if no bosses arrives or arrive with 0 life is intended as GAME MODE WITHOUT BOSSES
        // exponential distribution
        if (arrival_distribution == 0) {
            arrival_time = exponential(arrival_mean, arrival_rng);
        }
        // constant distribution
        else {
            arrival_time = arrival_mean;
        }

        scheduleAt(simTime() + arrival_time, timer_);

        EV << "BOSS - scheduleNextArrival() - new opponent arrives at: " << simTime()+arrival_time << endl;
    }
    else{
        EV << "BOSS - scheduleNextArrival() -  GAME MODE WITHOUT BOSSES " << endl;
    }
}


/**
 * Generate new opponent and set the service time (opponent life)
 */
void Boss::generateNewOpponent(){
    simtime_t service_time;

    if (service_distribution == 0) {
        service_time = exponential(service_mean, service_rng);
    }
    else {
        service_time = service_mean;
    }

    // Send the new opponent to the player as a message containing the service time (opponent life)
    OpponentMessage* msg = new OpponentMessage();
    msg->setService_time(service_time);
    msg->setName("BossMessage");
    send(msg, "out");

    EV << "BOSS - generateNewOpponent() - generated new opponent. life = " << service_time << endl;


}

/**
 * Overrides parent's finish method,
 * in order to deallocate dynamic memory before recording statistics
 */
void Boss::finish()
{
    //delete timer
    if (this == timer_->getOwner()) {
        EV << "Delete timer_" << endl;
        delete timer_;
    }

    cSimpleModule::finish();
}
