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

#include "Minion.h"

Define_Module(Minion);

void Minion::initialize()
{
    //initialize parameters
    arrival_mean = par("arrival_mean");
    service_mean = par("service_mean");
    arrival_distribution = par("arrival_distribution");
    service_distribution = par("service_distribution");

    timer_ = new cMessage("timer");

    //first arrival
    wait_new_arrival();
}

void Minion::handleMessage(cMessage *msg)
{
    //generate new MINION
    generate_new_opponent();

    //wait new arrival
    wait_new_arrival();
}

//Wait for a new opponent arrival (random time), extracted from an exponential distribution
void Minion::wait_new_arrival(){
    simtime_t arrival_time;

    // exponential distribution
    if (arrival_distribution == 0) {
        arrival_time = exponential(arrival_mean, arrival_rng);
    }
    // constant distribution
    else {
        arrival_time = arrival_mean;
    }

    scheduleAt(simTime() + arrival_time, timer_);

    EV << "MINION - new opponent arrives at: " << simTime()+arrival_time << endl;
}

//generate new opponent and set the service time (opponent life)
void Minion::generate_new_opponent(){
    simtime_t service_time;
    //to handle the degeneracy test
    if(arrival_mean != 0 && service_mean != 0){
        if (service_distribution == 0) {
            service_time = exponential(service_mean, service_rng);
        }
        else {
            service_time = service_mean;
        }

        // Send the new opponent to the player as a message containing the service time (opponent life)
        OpponentMessage* msg = new OpponentMessage();
        msg->setService_time(service_time);
        //msg->setType(1); //0 is a boss, 1 is a minion
        msg->setName("MinionMessage");
        send(msg, "out");

        EV << "MINION - generated new opponent. life = " << service_time << endl;
    }
    else{
        EV << "MINION - no minions, not possible to play"<< endl;
        endSimulation();
    }


}


/**
 * Overrides parent's finish method,
 * in order to deallocate dynamic memory before recording statistics
 */
void Minion::finish()
{
    //delete timer
    if (this == timer_->getOwner()) {
        EV << "Delete timer_" << endl;
        delete timer_;
    }

    cSimpleModule::finish();
}



