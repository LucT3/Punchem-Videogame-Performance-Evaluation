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

#include "Player.h"

Define_Module(Player);

void Player::initialize()
{
    //initialize parameters
    minion = new cMessage("MinionMessage");
    boss = new cMessage("BossMessage");
    recover_rate_x = par("recover_rate_x");
    counter_minion_defeated = par("counter_minion_defeated");
    counter_minion_recovered = 0;
    counter_boss_defeated = par("counter_boss_defeated");
    warmup_period = getSimulation()->getWarmupPeriod();

    //initialize current opponent
    current_opponent = nullptr;


    //MINION signals
    signal_minion_response_time = registerSignal("signal_minion_response_time");
    signal_minion_waiting_time = registerSignal("signal_minion_waiting_time");
    signal_minion_recovered = registerSignal("signal_minion_recovered");
    signal_minion_defeated = registerSignal("signal_minion_defeated");
    signal_minion_throughput = registerSignal("signal_minion_throughput");

    //record initial state
    emit(signal_minion_recovered,0);
    emit(signal_minion_defeated,0);


    //BOSS signals
    signal_boss_response_time = registerSignal("signal_boss_response_time");
    signal_boss_waiting_time = registerSignal("signal_boss_waiting_time");
    signal_boss_defeated = registerSignal("signal_boss_defeated");
    signal_boss_throughput = registerSignal("signal_boss_throughput");

    //record initial state
    emit(signal_boss_defeated,0);

}

/**
 * function that provides the following operations:
 *  1)handle incoming messages (MINIONS or BOSSES): queuing or defeating them.
 *  2)recover minion life if a BOSS arrive and a MINION is under service(recoverMinion function)
 *  3)emit signals to store statistics about minion/bosses number of jobs
 *
 */
void Player::handleMessage(cMessage *msg){

    //check the message type
    EV << "PLAYER - handleMessage() - message type: " << msg->getName() << " id: "<< msg->getId() << endl;

    //service time ends - DEFEAT OPPONENT
    if(msg->isSelfMessage()){
        EV << "PLAYER - handleMessage() - DEFEAT THE CURRENT OPPONENT " << endl;
        defeatOpponent(msg);

        if(!boss_queue.empty()){
            handleBoss();
        }
        else if (!minion_queue.empty()){
            handleMinion();
        }
        else{
            EV << "PLAYER - handleMessage() - no more opponents available at the moment " << endl;
        }
    }

    //else, new opponent arrives
    else{
        Opponent* new_opponent = new Opponent{msg, simTime()};

        std::string message_type = msg->getName();
        EV << "PLAYER - handleMessage() - NEW OPPONENT CREATED, type: " << message_type << endl;

        //enqueue boss msg in BOSS QUEUE and if necessary recover minion
        if(message_type == "BossMessage"){
            //push boss in the queue
            boss_queue.push(new_opponent);
            EV << "PLAYER - handleMessage() - ENQUEUED IN BOSS QUEUE. bosses:  "<< boss_queue.size() << endl;

            //check if a minion is been serving and skip its process (do the minion recover)
            if(current_opponent != nullptr){
                std::string current_opponent_type = current_opponent->message->getName();
                if(current_opponent_type == "MinionMessage"){
                    recoverMinion();
                }
            }
            //handle boss job
            if(boss_queue.size() == 1 ){
                if (minion_queue.size() > 0){ //if minion is serving cancel its scheduled event
                    cancelEvent(minion);
                }
                handleBoss();
            }
        }
        //enqueue minion msg in MINION QUEUE
        else if(message_type == "MinionMessage"){
            minion_queue.push(new_opponent);
            EV << "PLAYER - handleMessage() - ENQUEUED IN MINION QUEUE. minions: " << minion_queue.size() << endl;

            //handle minion job
            if(minion_queue.size() == 1 && boss_queue.size() == 0){
                handleMinion();
            }
        }
        //ERROR case
        else{
            EV <<"PLAYER - handleMessage() - error occurred on new opponent arriving! " << endl;
        }
    }
}


/**
 * handle a MINION opponent.
 * Process minion message and emit signals to store statistics
 */
void Player::handleMinion(){
    //take the first element of the MINION QUEUE and scheduleAt new simtime to process it
    EV << "PLAYER - handleMinion()" << endl;

    current_opponent = minion_queue.front();
    OpponentMessage *msg = check_and_cast<OpponentMessage*>(current_opponent->message);
    current_opponent_simTime = simTime();
    current_opponent_lifetime = msg->getService_time();
    EV <<"PLAYER - handleMinion() - Minion life : " << msg->getService_time() << endl;
    EV <<"PLAYER - handleMinion() - Minion defeat time : " << simTime() + msg->getService_time() << endl;

    scheduleAt(simTime() + msg->getService_time(), minion);

    //job enter in the service - record the waiting time
    emit(signal_minion_waiting_time, simTime() - current_opponent->enter_queue_time);
    EV <<"PLAYER - handleMinion() - Current opponent Enter queue time: " << current_opponent->enter_queue_time << endl;

}


/**
 * handle a BOSS opponent.
 * Process boss message and emit signals to store statistics
 */
void Player::handleBoss(){
    //take the first element of the BOSS QUEUE and scheduleAt new simtime to process it
    EV << "PLAYER - handleBoss()" << endl;

    current_opponent = boss_queue.front();
    OpponentMessage* msg = check_and_cast<OpponentMessage*>(current_opponent->message);
    EV <<"PLAYER - handleBoss() - Boss defeat time : " << simTime() + msg->getService_time()<< endl << endl;

    scheduleAt(simTime() + msg->getService_time(), boss);

    //job enter in the service - record the waiting time
    emit(signal_boss_waiting_time, simTime() - current_opponent->enter_queue_time);
    EV <<"PLAYER - handleBoss() - Current opponent Enter queue time: " << current_opponent->enter_queue_time << endl;


}

/**
 * Recover Minion function:
 *      1)call the function to compute the life recovered
 *      2)enqueue the recovered minion (deallocating safely the memory)
 *
 */
void Player::recoverMinion(){
    EV << "PLAYER - recoverMinion()" << endl;

    //check the current opponent
    EV << "PLAYER - recoverMinion() - RECOVERING MINION: " << current_opponent->message->getName() << " id: " << current_opponent->message->getId() << endl;

    //COMPUTE the recovered life
    simtime_t life_recovered = computeLifeRecovered();

    //CREATE NEW MINION and set the new actual life
    OpponentMessage* minion_recovered_msg = new OpponentMessage();
    minion_recovered_msg->setService_time(life_recovered);
    minion_recovered_msg->setName("MinionMessage");

    //ENQUEUE THE NEW MINION created
    Opponent* minion_recovered = new Opponent{minion_recovered_msg, simTime()};
    minion_queue.push(minion_recovered);

    //DELETE THE RECOVERED MINION from the queue
    EV <<"PLAYER - recoverMinion() - MINION DELETED FROM THE QUEUE, id : " << minion_queue.front()->message->getId() << endl;
    Opponent *defeated_opponent = minion_queue.front();
    minion_queue.pop();
    //deallocate memory
    delete defeated_opponent->message;
    delete defeated_opponent;

    EV << "PLAYER - recoverMinion() - MINION CORRECTLY RECOVERED/RE-ENQUEUED. life: "<< life_recovered << " minions in the queue : "<< minion_queue.size() << endl;

    //UPDATE current_opponent (boss that is arrived)
    current_opponent = boss_queue.front();
    OpponentMessage* msg = check_and_cast<OpponentMessage*>(current_opponent->message);
    current_opponent_simTime = simTime();
    current_opponent_lifetime = msg->getService_time();


    //collect statistics on recovery (only if the warm up time period is over)
    if (warmup_period < simTime()){
        counter_minion_recovered = counter_minion_recovered + 1;
        emit(signal_minion_recovered,counter_minion_recovered);
    }
    else{
        EV << "PLAYER - recoverMinion() - minion recovered not recorded, time left to recording: "<< (warmup_period-simTime()) << endl;
    }


}

/**
 * Function to compute the recovered minion life (life = service_time)
 * minion recovering steps:
 *      1)compute the lost life (gap between current simTime and start process simTIme)
 *      2)compute the remaining life percentage (based on the original life)
 *      3)compute the recovering percentage to add to the actual life
 *          3.1)no minion life recover (recover rate <= 0)
 *          3.2)100% of the recovering percentage (recover rate >= 100)
 *          3.3)x % of the recovering percentage (recover rate  0 < x < 100)
 *      4)update the actual life
 *
 */
simtime_t Player::computeLifeRecovered(){

    //1)compute the minion actual life
    simtime_t simTime_gap = simTime() - current_opponent_simTime; //difference between the current simTime and the fight start simTime
    simtime_t actual_life = current_opponent_lifetime - simTime_gap; //minion actual life (when it stops to fight)
    EV << "PLAYER - computeLifeRecovered() - original life : " << current_opponent_lifetime << ", actual life: " << actual_life << endl;

    //2)compute the current life percentage
    double current_opponent_life_percentage = 0; //minion actual life percentage
    if(actual_life > 0){
        current_opponent_life_percentage = (actual_life / current_opponent_lifetime) * 100;
    }
    //case in which the gap between simTimes is greater than the life/service time (NOT NEEDED)
    else{
        current_opponent_life_percentage = 90; //standard case (recover the 10% of the actual life)
    }
    EV << "PLAYER - computeLifeRecovered() - actual life percentage : " << current_opponent_life_percentage << "%" << endl;

    //3)compute the recovering percentage to add to the actual life (based on the chosen recover rate "x")
    double recovering_percentage;

    if(recover_rate_x <= 0){ //3.1) "no minion life recover"
        recovering_percentage = 0;
    }
    else if(recover_rate_x >= 100){ //3.2) "100% recover rate"
        recovering_percentage = 100 - current_opponent_life_percentage; //all percentage that he lost (not possible to recover more than the 100%)
    }
    else{ //recover_rate_x > 0, //3.3) "x% recover rate"
            recovering_percentage = ((100 - current_opponent_life_percentage) / 100) * recover_rate_x;

    }

    EV << "PLAYER - computeLifeRecovered() - percentage to recover : " << recovering_percentage << "%" << endl;

    //4)update the actual life
    simtime_t recovered_life = (actual_life / 100) * recovering_percentage; //life to add to the minion actual life
    actual_life = actual_life + recovered_life;
    EV << "PLAYER - computeLifeRecovered() - MINION LIFE RECOVERED : " << actual_life << "  Life added : " << recovered_life << "  Minion id : " << current_opponent->message->getId() << endl;

    return actual_life;
}


/**
 * Function to delete opponents (boss or minion) from queue, update the current opponent and
 * collect statistics (opponents defeated and response time)
 */
void Player::defeatOpponent(cMessage *msg){
    //check the opponent type to defeat
    EV << "PLAYER - defeatOpponent()" << endl;
    EV << "PLAYER - defeatOpponent() - opponent type : " << msg->getName() << " id: " << msg->getId() << endl;
    std::string msg_type = msg->getName();

    Opponent *defeated_opponent = nullptr;

    //defeat a BOSS
    if (msg_type == "BossMessage"){
        defeated_opponent = boss_queue.front();
        boss_queue.pop();
        EV << "PLAYER - defeatOpponent() - BOSS defeated. remaining bosses: "<< boss_queue.size() << endl;

        //collect statistics (only if the warm up time period is over)
        if (warmup_period < simTime()){
            counter_boss_defeated = counter_boss_defeated + 1;
            emit(signal_boss_defeated, counter_boss_defeated);
            EV << "PLAYER - defeatOpponent() - Total Bosses Defeated : "<< counter_boss_defeated << endl;
        }
        else{
            EV << "PLAYER - defeatOpponent() - Boss defeated not recorded, time left to recording: "<< (warmup_period-simTime()) << endl;
        }

    }
    //defeat a MINION
    else if(msg_type == "MinionMessage"){
        defeated_opponent = minion_queue.front();
        minion_queue.pop();
        EV << "PLAYER - defeatOpponent() - MINION defeated. remaining minions:  "<< minion_queue.size() << endl;

        //collect statistics (only if the warm up time period is over)
        if (warmup_period < simTime()){
            counter_minion_defeated = counter_minion_defeated + 1;
            emit(signal_minion_defeated,counter_minion_defeated);
            EV << "PLAYER - defeatOpponent() - Total Minion Defeated : "<< counter_minion_defeated << endl;
        }
        else{
            EV << "PLAYER - defeatOpponent() - Minion defeated not recorded, time left to recording: "<< (warmup_period-simTime()) << endl;
        }
    }
    else{
        EV << "PLAYER - defeatOpponent() - ERROR!" << endl;
    }

    //collect statistics on response time
    if (msg_type == "BossMessage"){
        emit(signal_boss_response_time, simTime() - defeated_opponent->enter_queue_time);
        EV << "PLAYER - defeatOpponent() - Boss response time: " << simTime() - defeated_opponent->enter_queue_time << endl;
    }
    else{
        emit(signal_minion_response_time, simTime() - defeated_opponent->enter_queue_time);
        EV << "PLAYER - defeatOpponent() - Minion response time: " << simTime() - defeated_opponent->enter_queue_time << endl;
    }

    //memory deallocation
    delete defeated_opponent->message;
    delete defeated_opponent;
    current_opponent = nullptr;
}


/**
 * Override the finish method, in order to compute the final statistics and deallocate memory
 */
void Player::finish(){
    EV << "PLAYER - finish() - GAME OVER! " << endl;

    //compute final statistics (defeated minions and bosses per unit of time [seconds])
    emit(signal_minion_throughput, (double) counter_minion_defeated / (simTime()-warmup_period));
    EV << "PLAYER - finish() - TOTAL MINIONS DEFEATED: "<< counter_minion_defeated <<". MINION THROUGHPUT : " << counter_minion_defeated / (simTime()-warmup_period) <<". ON SIMULATION TIME : " << (simTime()-warmup_period) << endl;

    emit(signal_boss_throughput, (double)counter_boss_defeated / (simTime()-warmup_period));
    EV << "PLAYER - finish() - TOTAL BOSSES DEFEATED: "<< counter_boss_defeated <<". BOSS THROUGHPUT : " << counter_boss_defeated / (simTime()-warmup_period) <<". ON SIMULATION TIME : " << (simTime()-warmup_period) << endl;

    //delete minion queue
    while (!minion_queue.empty()){
        Opponent *opponent = minion_queue.front();
        minion_queue.pop();
        delete opponent->message;
        delete opponent;
    }

    //delete boss queue
    while (!boss_queue.empty()){
        Opponent *opponent = boss_queue.front();
        boss_queue.pop();
        delete opponent->message;
        delete opponent;
    }

    if(minion_queue.empty() && boss_queue.empty()){
        EV << "Deallocated Memory" << endl;
    }

    //delete timers
    cancelAndDelete(minion);
    cancelAndDelete(boss);

    cSimpleModule::finish();

}



