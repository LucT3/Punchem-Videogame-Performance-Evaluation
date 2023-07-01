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

#ifndef __PUNCHEM_LUCA_PLAYER_H_
#define __PUNCHEM_LUCA_PLAYER_H_

#include <omnetpp.h>
#include <queue>
#include <string>
#include <iostream>
#include "OpponentMessage_m.h"

using namespace omnetpp;


struct Opponent{
    cMessage* message;
    simtime_t enter_queue_time;
};


class Player : public cSimpleModule
{

    //parameters
    unsigned int counter_minion = 0;
    unsigned int counter_boss = 0;
    double recover_rate_x;

    unsigned int counter_minion_defeated = 0;
    unsigned int counter_minion_recovered = 0;
    unsigned int counter_boss_defeated = 0;
    simtime_t warmup_period = 0;

    Opponent *current_opponent = nullptr;
    simtime_t current_opponent_simTime;
    simtime_t current_opponent_lifetime;
    cMessage *minion = nullptr; //timer for minion messages
    cMessage *boss = nullptr; //timer for boss messages


    //queues for minions and bosses
    std::queue <Opponent*> minion_queue;
    std::queue <Opponent*> boss_queue;


    //signals
    simsignal_t signal_minion_response_time = 0;
    simsignal_t signal_minion_waiting_time = 0;
    simsignal_t signal_minion_defeated = 0;
    simsignal_t signal_minion_recovered = 0;
    simsignal_t signal_minion_throughput;


    simsignal_t signal_boss_response_time = 0;
    simsignal_t signal_boss_waiting_time = 0;
    simsignal_t signal_boss_defeated = 0;
    simsignal_t signal_boss_throughput;


    //functions
    void handleMinion();
    void handleBoss();

    void recoverMinion();
    simtime_t computeLifeRecovered();

    void defeatOpponent(cMessage *msg);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
