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

package punchem_luca.simulations;

import punchem_luca.Boss;
import punchem_luca.Minion;
import punchem_luca.Player;


network Network
{
    @display("bgb=361,193");
    submodules:
        minion: Minion {
            @display("p=50.235,89.635");
        }
        player: Player {
            @display("p=172,153");
        }
        boss: Boss {
            @display("p=290.575,89.635");
        }
    connections:
        minion.out --> player.minion_in;
        boss.out --> player.boss_in;
}
