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
        double recover_rate_x = default(100); //<=0 is "no minion life recover ", 1<= x <= 99  is "x% recover rate", x >= 100 is "100% of recover rate"
        int counter_minion_defeated = default(0);
        int counter_boss_defeated = default(0);
        
        //MINION PARAMETERS
        @signal [signal_minion_jobs_number];
        @statistic [minion_jobs_number_stat] (source="signal_minion_jobs_number"; record = mean, vector);
        
        @signal [signal_minion_jobs_queue_number];
        @statistic [minion_jobs_queue_number_stat] (source="signal_minion_jobs_queue_number"; record = vector);
        
        //to measure the minion response time
        @signal [signal_minion_response_time];
        @statistic [minion_response_time_stat] (source="signal_minion_response_time"; record = mean, vector);
        
        //to measure the minion waiting time
        @signal [signal_minion_waiting_time];
        @statistic [minion_waiting_time_stat] (source="signal_minion_waiting_time"; record = mean, vector);
        
        @signal [signal_minion_defeated];
        @statistic [minion_defeated_stat] (source = "signal_minion_defeated" ; record = count, mean, vector);
        
        @signal [signal_minion_recovered];
        @statistic [minion_recovered_stat] (source = "signal_minion_recovered" ; record = count, mean, vector);
        
        //to measure the throughput (in terms of minions defeated per unit of time)
        @signal[signal_minion_throughput](type="long");
        @statistic [minion_throughput] (source = "signal_minion_throughput"; record = vector, mean, last);
        
        
        
        //BOSS PARAMETERS
        @signal [signal_boss_jobs_number];
        @statistic [boss_jobs_number_stat] (source="signal_boss_jobs_number"; record = vector);
        
        @signal [signal_boss_jobs_queue_number];
        @statistic [boss_jobs_queue_number_stat] (source="signal_boss_jobs_queue_number"; record = vector);
        
        @signal [signal_boss_response_time];
        @statistic [boss_response_time_stat] (source="signal_boss_response_time"; record = mean, vector);
        
        @signal [signal_boss_waiting_time];
        @statistic [boss_waiting_time_stat] (source="signal_boss_waiting_time"; record = mean, vector);
        
        @signal [signal_boss_defeated];
        @statistic [boss_defeated_stat] (source = "signal_boss_defeated" ; record = count, mean, vector);
        
        @signal[signal_boss_throughput](type="long");
        @statistic [boss_throughput] (source = "signal_boss_throughput"; record = vector, mean, last);
       	
        
             
	gates:
	    input boss_in;
	    input minion_in;    
}
