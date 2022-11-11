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

#ifndef __PUNCHEM_LUCA_BOSS_H_
#define __PUNCHEM_LUCA_BOSS_H_

#include <omnetpp.h>
#include "OpponentMessage_m.h"

// Index of the Random Number Generators
#define arrival_rng 0
#define service_rng 1

using namespace omnetpp;

class Boss : public cSimpleModule
{
    // distribution parameters
    double arrival_mean = 0;
    double service_mean = 0;

    unsigned int arrival_distribution = 0;
    unsigned int service_distribution = 0;

    //messages
    cMessage* timer_ = nullptr;

    //functions
    void wait_new_arrival();
    void generate_new_opponent();



  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
