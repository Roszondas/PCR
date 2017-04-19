Project description:
This is a code for AI-agents in a "Coders of the Caribbean" game.


CODERS OF THE CARIBBEAN

Game page:<br>
https://www.codingame.com/contests/coders-of-the-caribbean

Some gameplay examples:<br>
https://www.codingame.com/replay/209169597<br>
https://www.codingame.com/replay/209158698<br>
https://www.codingame.com/replay/209131096<br>
https://www.codingame.com/replay/209111281<br>
https://www.codingame.com/replay/208944613<br>


The Goal

In this game, you are in command of pirate ships and your goal is to avoid running out of rum. If all the rum dries up on a given ship, your crew will go mad and you will lose the ship. Barrels of rum are placed randomly across the map. You must control the movements of your ship in order to collect the barrels of rum before your opponent does.
 


Rules

The game is played on a hexagonal grid 23 cells wide and 21 high.

Both players have one ship (and up to 3 in later leagues). Every ship is 3 cells long and 1 cell wide.

On the grid you will find barrels of rum (BARREL). The barrels contain between 10 and 20 units of rum. Each ship can carry a maximum of 100 units of rum (surplus will be lost).

Each turn, the players can decide to move their ship towards the cell of their choice using the MOVE command. Ships can gain speed, slow down, turn left (port) or right (starboard). The MOVE action uses a very simplified algorithm to reach the destination.

Ships can place mines on the grid with the MINE command. This action spawns a mine in the cell directly behind the ship. After placing a mine, a ship cannot place another for the next 4 turns.

If a mine is touched by a ship or hit by a cannon ball, it explodes. A ship touching a mine loses 25 units of rum. A ship in a cell adjacent to an exploding mine loses 10 units of rum. Players can only see mines if they are at a distance less than 5 cells away from the center of one of their ships.

A ship can fire a cannon ball with the FIRE x y command where x and y are the grid coordinates of the target cell. The target must be within 10 cells of the front of the ship. The cannon ball is launched from the front of the ship and will take 1 + (distance to target) / 3 turns to reach the target (the result is rounded). If the cannon ball lands on the front or back of a ship, that ship loses 25 units. The ship loses 50 units if the cannon ball lands on its center. After shooting a cannon ball, a ship cannot fire during the next turn.


Note: when using a command such as MINE, FIRE and WAIT, the ship will still be moving with the same direction and speed as last turn.





Game turns:

One game turn is computed as follows:
The amount of rum each ship is carrying is decreased by 1 unit.
The players' commands are applied (spawning of cannon balls, mines and ship movement).
Ships move forward by the same number of cells as their speed.
Ships turn.
Damage from cannon balls is computed.
Elimination of ships with no more rum.
If at any point during its movement a ship shares a cell with a barrel of rum, the ship collects that rum. In the same way, if a ship touches a mine, that mine explodes and the loss of rum is applied immediately.





The grid:

The (0,0) coordinate is at the top left corner of the grid. The game's grid is made up of hexagonal cells in which odd lines are slightly shifted compared to a normal grid. Since a hexagon has 6 sides, each cell had 6 neighbors (except cells on the edges of the map).

The distance between two cells equals the minimum number of cells to go through to get from one to the other.



The ships:

A ship's position in the grid is represented by the coordinates of its center and its direction. The direction is a number between 0 and 5.




Victory Conditions
Your opponent runs out of rum.
After 200 turns, you have more rum in total than your opponent.




Expert Rules

Collisions

If a ship attempts to leave the map, it is stopped and its speed is set to 0.

Collisions between ships are computed during the movement phase. If a moving ship's front were to collide with another ship, its movement is cancelled and its speed is set to 0. Every time a movement is cancelled, collisions are checked again using the new positions.

For ships with a speed of 2, there are two stages in the movement phase. The first stage concerns all ships with speed ≥ 1, and the second concerns only ships with speed = 2). The collision check is performed at the end of each stage.

At the end of the ships' rotation phase, collision are checked again. Ships whose rotation would bring about a collision will have their rotation cancelled and their speed set to 0. Collisions are rechecked each time a rotation is cancelled.





Manual steering of a ship:
 
Starting at Bronze league, you can control the actions of your ship manually with the following commands: FASTER, SLOWER, PORT, STARBOARD.

The FASTER and SLOWER commands will increase or decrease the speed of your ship by 1 unit. The value of the speed can vary from 0 to 2.

The PORT and STARBOARD commands will change the rotation of your ship (on the next turn) by turning it respectively left or right.




Source code:

The source code of the game is available on our Github at this address: https://github.com/CodinGame/coders-of-the-caribbean/<br><br><br>





Game Input

Input for one game turn<br>
Line 1: myShipCount, an integer for the number of ships you control.<br>
Line 2 : entityCount, an integer for the number of entities on the grid.<br>
The next entityCount lines: an integer entityId, a string entityType, two integers x and y for the coordinates of the entity, then 4 integers arg1, arg2, arg3 and arg4.<br>

If entityType equals SHIP, then the integers represent:<br>
arg1: the ship's rotation orientation (between 0 and 5)<br>
arg2: the ship's speed (between 0 and 2)<br>
arg3: the ship's stock of rum units<br>
arg4: 1 if the ship is controlled by you, 0 otherwise

If entityType equals BARREL, then the integers represent:<br>
arg1: the amount of rum in this barrel

If entityType equals CANNONBALL, the x and y represent the target coordinates and the integers represent:<br>
arg1: the entityId of the ship that fired this cannon ball<br>
arg2: the number of turns before impact (1 means the cannon ball will land at the end of the current turn)

If entityType equals MINE, then the integers have no meaning.<br><br><br>




Output for one game turn<br>
For each one of your ships, one of the following commands:<br>
MOVE x y: move automatically towards the specified coordinate.<br>
FIRE x y: launch a cannon ball to the specified coordinate.<br>
MINE: place a mine on the cell behind the ship.<br>
PORT: turn the ship left.<br>
STARBOARD: turn the ship right.<br>
FASTER: speed the ship up.<br>
SLOWER: speed the ship down.<br>
WAIT: perform no action.<br>
The commands must be given in the same order as how the ships were given as input (the entityId are in ascending order).<br><br><br>



Constraints<br>
1 ≤ myShipCount ≤ 3<br>
10 ≤ rumBarrelCount ≤ 26<br>
5 ≤ initialMineCount ≤ 10<br>
0 ≤ x ≤ 22<br>
0 ≤ y ≤ 20<br>
0 ≤ shipSpeed ≤ 2<br>
Response time for first turn ≤ 1000ms<br>
Response time for one turn ≤ 50ms
