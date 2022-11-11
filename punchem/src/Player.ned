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

package punchem_luca;

//
// TODO auto-generated module
//
simple Player
{       
    parameters:
        double recover_rate_x = default(0); //<=0 is "no minion life recover ", 1<= x <= 99  is "x% recover rate", x >= 100 is "100% of recover rate"
        
        
        //MINION PARAMETERS
        @signal [signal_minion_jobs_number];
        @statistic [minion_jobs_number_stat] (title="number of overall minions"; source="signal_minion_jobs_number"; record=vector);
        
        @signal [signal_minion_jobs_queue_number];
        @statistic [minion_jobs_queue_number_stat] (source="signal_minion_jobs_queue_number"; record=vector);
        
        @signal [signal_minion_response_time];
        @statistic [minion_response_time_stat] (source="signal_minion_response_time"; record=vector);
        
        @signal [signal_minion_waiting_time];
        @statistic [minion_waiting_time_stat] (source="signal_minion_waiting_time"; record=vector);
        
        
        //BOSS PARAMETERS
        @signal [signal_boss_jobs_number];
        @statistic [boss_jobs_number_stat] (source="signal_boss_jobs_number"; record=vector);
        
        @signal [signal_boss_jobs_queue_number];
        @statistic [boss_jobs_queue_number_stat] (source="signal_boss_jobs_queue_number"; record=vector);
        
        @signal [signal_boss_response_time];
        @statistic [boss_response_time_stat] (source="signal_boss_response_time"; record=vector);
        
        @signal [signal_boss_waiting_time];
        @statistic [boss_waiting_time_stat] (source="signal_boss_waiting_time"; record=vector);
        
             
	gates:
	    input boss_in;
	    input minion_in;    
}
