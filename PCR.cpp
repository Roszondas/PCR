#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <map>
#include <queue>
#include <chrono>
#include <list>
#include <math.h>

using namespace std;

#define INF        999999
#define INF_NEG    -999999

#define ARGS_NUM            4
#define ENTITY_TYPES_NUM    6
#define WIDTH               23
#define HEIGHT              21

#define NOSE_OFSET          6
#define BACK_OFSET          7

#define _TIMER chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()

enum {CMD_WAIT, CMD_LEFT, CMD_RIGHT, CMD_FASTER, CMD_SLOWER, CMD_FIRE, CMD_MINE, CMD_MOVE};
enum {SHIP, BARREL, EXPLOSION, MINE, SEA};
enum {SHIP_ROTATION, SHIP_SPEED, SHIP_RUM, SHIP_OWNER};
enum {OWNER_FOE, OWNER_ME};
enum {BOMB_OWNER, BOMB_TIMER};



string entity_name[ENTITY_TYPES_NUM] {"SHIP", "BARREL", "CANNONBALL", "MINE", "SEA", "PART"};
int entity_type[ENTITY_TYPES_NUM] {SHIP, BARREL, EXPLOSION, MINE, SEA};

int DIRECTIONS_EVEN[6][2] { { 1, 0 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 } };
int DIRECTIONS_ODD[6][2]  { { 1, 0 }, { 1, -1 }, { 0, -1 },  { -1, 0 }, { 0, 1 },  { 1, 1 } };


///////////////////////////////////// GridPoint / CubePoint
struct CubePoint;

struct GridPoint {
    int x, y;
    
    GridPoint () {
        x = -1;
        y = -1;
    }
    
    GridPoint (int x, int y){
        this->x = x;
        this->y = y;
    }
    
    GridPoint (const GridPoint &other){
        this->x = other.x;
        this->y = other.y;
    }
    
    GridPoint (const CubePoint &cube);
    
    double Angle(GridPoint targetPosition);
    
    bool isInsideMap() {
        if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) 
            return true;
        else
            return false;
    }
    
    bool operator==  (const GridPoint& other) const{
        if(this->x == other.x && this->y == other.y) return true; else return false;
    };
    
    bool operator!=  (const GridPoint& other) const{
        if(this->x == other.x && this->y == other.y) return false; else return true;
    };
    
    GridPoint& operator+=(const GridPoint& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    
    friend GridPoint operator+(GridPoint left, const GridPoint& right) {
        left += right;
        return left;
    }
};



struct CubePoint {
    int x, y, z;
    
    CubePoint() {
        x = -1;
        y = -1;
        z = -1;
    }
    
    CubePoint (int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    CubePoint (const CubePoint &other){
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }
    
    CubePoint (const GridPoint &grid){
        this->x = grid.x - (grid.y - (grid.y&1)) / 2;
        this->z = grid.y;
        this->y = -x-z;
    }
};


GridPoint::GridPoint (const CubePoint &cube)
{
        this->x = cube.x + (cube.z - (cube.z&1)) / 2;
        this->y = cube.z;
}



///////////////////////////////////// Entity
struct Entity
{
    int id;
    string type;
    GridPoint coords;
    int args[ARGS_NUM];
    
    Entity() {};
    
    Entity(int id, string type, GridPoint coords){
        this->id = id;
        this->type = type;
        this->coords = coords;
    }
};



///////////////////////////////////// Cell
struct Cell
{
    int index;
    Entity *content;
    int explode = -1;
    int visited = 0;
    int direction = -10;
    int speed = -10;
    
    Cell(int index, Entity *content) {
        this->index = index;
        this->content = content;
    }
};



///////////////////////////////////// CBoard
class CBoard
{
public:
    vector <Cell> grid;
    Entity voidSea;
    
    map <int, Entity> myShips;
    map <int, Entity> foeShips;
    map <int, Entity> barrels;
    
    void ClearAll();
    
//public:
    CBoard();
    int CheckCell(GridPoint point);
};


CBoard::CBoard()
{
    voidSea.type = "SEA";
    
    for(int i = 0; i < HEIGHT*WIDTH; i++)
        grid.push_back(Cell(i, &voidSea));
}


int CBoard::CheckCell(GridPoint point)
{
    int index = point.x + WIDTH*point.y;
    int ret = -1;
    
    if(grid[index].explode > -1) return EXPLOSION;
    
    for(int currType = 0; currType < ENTITY_TYPES_NUM; currType++) {
        if(grid[index].content->type == entity_name[currType]){
            ret = currType;
            break;
        }
    }
    
    return ret;
}


void CBoard::ClearAll()
{
    myShips.clear();
    foeShips.clear();
    barrels.clear();
}


///////////////////////////////////// PointWrapper
struct PointWrapper
{
    GridPoint coords;

    int keyValue;
    int cost;
    int direction;
    PointWrapper *from;
    
    PointWrapper(GridPoint coords, int cost, int direction, int keyValue, PointWrapper *parent){
        this->coords = coords;
        this->keyValue = keyValue;
        this->cost = cost;
        this->direction = direction;
        this->from = parent;
    }
    
    bool operator > (const PointWrapper& other) const{
        return (this->keyValue > other.keyValue);
    }
    
    bool operator < (const PointWrapper& other) const{
        return (this->keyValue < other.keyValue);
    }
    
    bool operator==  (const PointWrapper& other) const{
        if(this->keyValue == other.keyValue) return true; else return false;
    };
    
    bool operator!=  (const PointWrapper& other) const{
        if(this->keyValue == other.keyValue) return false; else return true;
    };
};



///////////////////////////////////// Order
struct Order
{
public:
    Order() {};
    
    Order(int cmd, int x, int y) {
        this->cmd = cmd;
        this->x = x;
        this->y = y;
        }
    
    Order(int cmd, GridPoint coords) {
        this->cmd = cmd;
        this->x = coords.x;
        this->y = coords.y;
        }
        
    Order(int cmd) {
        this->cmd = cmd;
    }
    
    void Message (string msg) {
        this->msg = msg;
    }
    
    string Out() {
        string ret = "";
        
        switch (cmd) {
            case CMD_MOVE:
                ret +=  (string) "MOVE "+
                to_string(x) +   " "    + 
                to_string(y);
                break;
            case CMD_FIRE:
                ret +=  (string) "FIRE "+
                to_string(x) +   " "    + 
                to_string(y);
                break;
            case CMD_MINE:
                ret +=  (string) "MINE";
                break;
            case CMD_LEFT:
                ret +=  (string) "PORT";
                break;
            case CMD_RIGHT:
                ret +=  (string) "STARBOARD";
                break;
            case CMD_FASTER:
                ret +=  (string) "FASTER";
                break;
            case CMD_SLOWER:
                ret +=  (string) "SLOWER";
                break;
            
            case CMD_WAIT:
            default:
                ret +=  (string) "WAIT";
        }

        if(!msg.empty())
            ret += " " + msg;

        return ret;
    };

private:
    int cmd = -1;
    int x = -1, y = -1;
    string msg = "";
};



///////////////////////////////////// CWorld
class CWorld{
private:
    Entity* currentShip;
    map <int, Entity> myShips;
    map <int, Entity> myShipsLast;
    
    map <int, Entity> foeShips;
    map <int, Entity> barrels;
    map <int, Entity> explosions;
    
    map <int, Entity> parts;
    map <int, Entity> fullEntityMap;
    
    int myShipCount = 1;
    int fireReloading[6] {0,0,0,0,0,0};
    int mineReloading[6] {0,0,0,0,0,0};
    
    void Output(vector <Order> &orders);
    Order DecisionMainSystem(int shipNum);
    GridPoint FindClosestBarrel(int shipNum);
    GridPoint FindClosestShootingTarget(int shipNum);
    int NavigationToTarget(Entity ship, GridPoint target);
    GridPoint FindPath(Entity ship, GridPoint target);
    
    int FindPathCmd(Entity& ship, GridPoint target);
    int ChkMov(GridPoint coords, int dir, int depth);
    
    int UpdateCost(GridPoint coords, int depth);
    
    GridPoint FindSpot(Entity ship, int desiredDist);
    
    void ClearAll();
    
public:
    CWorld() {};
    void UpdateEntity(Entity entity);
    void MakeTurn(int myShipCount);
    static int Distance(CubePoint a, CubePoint b);
    static int Distance(GridPoint a, GridPoint b);
    static int DirectionByNeighbor(GridPoint coords, GridPoint Nbr);
    static GridPoint NeighborByDirection(GridPoint coords, int direction);
    static GridPoint ApplySpeed(Entity ship, int turns);
    static GridPoint ApplySpeed(GridPoint coords, int direction, int speed, int turns);
};


int CWorld::Distance(CubePoint a, CubePoint b)
{
    return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2;
}


int CWorld::Distance(GridPoint a, GridPoint b)
{
    CubePoint ca = a;
    CubePoint cb = b;
    return Distance(ca, cb);
}


int CWorld::DirectionByNeighbor(GridPoint coords, GridPoint Nbr)
{
    int direction;
    int (*dirArray)[2];
    
    if (coords.y % 2 == 1) 
        dirArray = DIRECTIONS_ODD;
    else
        dirArray = DIRECTIONS_EVEN;
    
    for(int i = 0; i < 6; i++){
        if(Nbr.y == coords.y + dirArray[i][1] && Nbr.x == coords.x + dirArray[i][0]){
            direction = i;
            break;
        }
    }
    
    return direction;
};


GridPoint CWorld::NeighborByDirection(GridPoint coords, int direction)
{
    int (*dirArray)[2];
    
    if (coords.y % 2 == 1) 
        dirArray = DIRECTIONS_ODD;
    else
        dirArray = DIRECTIONS_EVEN;
        
    coords.y += dirArray[direction][1];
    coords.x += dirArray[direction][0];

    return coords;
};


GridPoint CWorld::ApplySpeed(Entity ship, int turns)
{
    if(ship.type != "SHIP") return ship.coords;
    
    for(int i = 0; i < (turns + 1) * ship.args[SHIP_SPEED]; i++){
        GridPoint chkCoords = NeighborByDirection(ship.coords, ship.args[SHIP_ROTATION]);
        if(chkCoords.isInsideMap())
            ship.coords = chkCoords;
        else
            break;
    }
    
    return ship.coords;
}


GridPoint CWorld::ApplySpeed(GridPoint coords, int direction, int speed, int turns)
{
    for(int i = 0; i < (turns + 1) * speed; i++){
        GridPoint chkCoords = NeighborByDirection(coords, direction);
        if(chkCoords.isInsideMap())
            coords = chkCoords;
        else
            break;
    }
    
    return coords;
}


void CWorld::UpdateEntity(Entity entity)
{
    int type = -1;
    
    for(int currType = 0; currType < ENTITY_TYPES_NUM; currType++) {
        if(entity.type == entity_name[currType]){
            type = currType;
            break;
        }
    }
    
    map <int, Entity> *container = nullptr;
    
    switch(type){
        case SHIP:
            {
                parts.insert(pair<int, Entity> (entity.id, entity));
                GridPoint fCoords = NeighborByDirection(entity.coords, entity.args[SHIP_ROTATION]);
                GridPoint bCoords = NeighborByDirection(entity.coords, (entity.args[SHIP_ROTATION] + 3)%6);
                string t = "PARTS";
                if(fCoords.isInsideMap()){
                    Entity front(entity.id + NOSE_OFSET, t, fCoords);
                    parts.insert(pair<int, Entity> (front.id, front));
                }
                if(bCoords.isInsideMap()){
                    Entity back(entity.id + BACK_OFSET, t, bCoords);
                    parts.insert(pair<int, Entity> (back.id, back));
                }
                
                if(entity.args[SHIP_OWNER] == OWNER_ME)
                    container = &myShips;
                else
                    container = &foeShips;
                    
                break;
            }
        case BARREL:
            container = &barrels;
            break;
            
        case EXPLOSION:
            container = &explosions;
            break;
            
        default:
            break;
    }
    
    if(container != nullptr)
        container->insert(pair<int, Entity> (entity.id, entity));
        
    // temporary solution
    GridPoint side;
    fullEntityMap.insert(pair<int, Entity> (entity.coords.x + WIDTH*entity.coords.y, entity));
    //
}


void CWorld::MakeTurn(int myShipCount)
{
    this->myShipCount = myShipCount;
    vector <Order> orders;

    auto T1 = _TIMER;
    
    for(auto &ships : myShips){
        if( fireReloading[ships.first] > 0) fireReloading[ships.first]--;
        if( mineReloading[ships.first] > 0) mineReloading[ships.first]--;
        orders.push_back(DecisionMainSystem(ships.first));
        orders.back().Message(to_string(ships.first));
    }
    
    auto T2 = _TIMER;
    cerr <<"T "<< T2 - T1 <<"ms"<<endl;
    
    this->Output(orders);
    
    ClearAll();
}


Order CWorld::DecisionMainSystem(int shipNum)
{
    int shipOrder = CMD_MOVE;
    
    GridPoint shipTargetCoords = FindClosestBarrel(shipNum);
    
    //fprintf(stderr, "Target %i %i\n", shipTargetCoords.x, shipTargetCoords.y);
    
    int maxRum = -1;
    Entity bestShip;
    for(auto &ship : myShips){
        if(ship.second.args[SHIP_RUM] > maxRum){
            maxRum = ship.second.args[SHIP_RUM];
            bestShip = ship.second;
        }
    }
    
    int closestDist = INF;
    Entity *closestEnemy;
    for(auto &currFoeShip : foeShips){
        GridPoint foeExpectedCoords = ApplySpeed(currFoeShip.second, 0);
        int dist = Distance(myShips[shipNum].coords, foeExpectedCoords);
        
        if(dist < closestDist) {
            closestDist = dist;
            closestEnemy = &currFoeShip.second;
        }
    }
    
    if(shipTargetCoords.x == -1){
        int dist;
        if(shipNum == bestShip.id) 
            dist = 300;
        else
            dist = 20;
            
        shipTargetCoords = FindSpot(myShips[shipNum], dist);
        // shipTargetCoords.x = rand() % WIDTH;
        // shipTargetCoords.y = rand() % HEIGHT;
        
        if(shipNum != bestShip.id && Distance(myShips[shipNum].coords, bestShip.coords) < 5
            && Distance(myShips[shipNum].coords, closestEnemy->coords) > 10
            && myShips[shipNum].args[SHIP_RUM] <= 50) {
            shipTargetCoords = ApplySpeed(myShips[shipNum], 0);
            shipOrder = CMD_FIRE;
            return Order(shipOrder, shipTargetCoords);
            }
    }
    
    
    //fprintf(stderr, "Target %i %i\n", shipTargetCoords.x, shipTargetCoords.y);
    //shipOrder = NavigationToTarget(myShips[shipNum], shipTargetCoords);
    
    shipOrder = FindPathCmd(myShips[shipNum], shipTargetCoords);
    
    // if(myShipsLast[shipNum].coords.x == myShips[shipNum].coords.x){
    //     shipOrder = CMD_WAIT;
    // }
    
    if(shipOrder == CMD_WAIT) {
        shipTargetCoords = FindClosestShootingTarget(shipNum);
        if(!fireReloading[shipNum] && shipTargetCoords.x != -1){
                shipOrder = CMD_FIRE;
        }
        else if(!mineReloading[shipNum]){
            if(Distance(myShips[shipNum].coords, closestEnemy->coords) < 6)
                shipOrder = CMD_MINE;
        }
    }

    if(shipOrder == CMD_FIRE && !fireReloading[shipNum]) fireReloading[shipNum] = 2;
    if(shipOrder == CMD_MINE && !mineReloading[shipNum]) mineReloading[shipNum] = 5;
    
    return Order(shipOrder, shipTargetCoords);
}



GridPoint CWorld::FindSpot(Entity ship, int desiredDist)
{
    //int desiredDist = 6;
    int movePrice = 0;
    GridPoint myCoords = ship.coords;
    int koef  =1;
    
    int estimate;
    int closestDist = INF;
    GridPoint bestCoords(INF, INF);
    Entity *closestEnemy;
    
    for(auto &currFoeShip : foeShips){
        GridPoint foeExpectedCoords = ApplySpeed(currFoeShip.second, 0);
        int dist = Distance(myCoords, foeExpectedCoords);
        
        if(dist < closestDist) {
            closestDist = dist;
            closestEnemy = &currFoeShip.second;
        }
    }
    
    CBoard Board;
    GridPoint newCoords = myCoords;
    priority_queue <PointWrapper> pQueue;
    list <PointWrapper> store;
    
    
    cerr << "Dist = " << Distance(closestEnemy->coords, myCoords) << endl;
    if(Distance(closestEnemy->coords, myCoords) > desiredDist)
        koef = -1;
    else
        koef = 1;
    
    pQueue.push(PointWrapper(newCoords, 0, ship.args[SHIP_ROTATION], 0, nullptr));
    
    while(!pQueue.empty()){
        PointWrapper curPoint = pQueue.top();
        store.push_back(curPoint);
        PointWrapper *parent = &store.back();
        pQueue.pop();
        
        //fprintf(stderr, "Check curPoint %i %i - %i\n", curPoint.coords.x, curPoint.coords.y, curPoint.keyValue);
        
        bestCoords.x = curPoint.coords.x;
        bestCoords.y = curPoint.coords.y;
        //fprintf(stderr, "Check curPoint %i %i - %i\n", curPoint.coords.x, curPoint.coords.y, curPoint.keyValue);
        
        if(!curPoint.coords.isInsideMap()) continue;
        if(koef*Distance(closestEnemy->coords, curPoint.coords) >= koef*desiredDist) break;
                
        for(int i = 0; i < 6; i++){
            int newCost = curPoint.cost + movePrice;
            if(i==curPoint.direction)
                newCost--;
    
            GridPoint nbrCoords = NeighborByDirection(curPoint.coords, i);
            if(!nbrCoords.isInsideMap()) continue;

            int priorityKey = -Distance(closestEnemy->coords, nbrCoords);
            
            if(Board.grid[nbrCoords.x + nbrCoords.y*WIDTH].visited) continue;
            
            pQueue.push(PointWrapper(nbrCoords, newCost, i, priorityKey, parent ));
            
            Board.grid[nbrCoords.x + nbrCoords.y*WIDTH].visited = 1;
            
            //fprintf(stderr, "nbr %i %i dist %i pri %i\n", nbrCoords.x, nbrCoords.y, (Distance(closestEnemy->coords, nbrCoords)), priorityKey);
            
        }

    }
    
    
    if(bestCoords.x == INF){
        bestCoords.x = -1;
        bestCoords.y = -1;
    }

    return bestCoords;
}


GridPoint CWorld::FindClosestBarrel(int shipNum)
{
    GridPoint myCoords = myShips[shipNum].coords;
    GridPoint bestCoords(INF, INF);

    for(auto currBarrel : barrels){
        GridPoint barrelCoords = currBarrel.second.coords;
        
        if(Distance(myCoords, barrelCoords) < Distance (myCoords, bestCoords))
            bestCoords = barrelCoords;
    }
    
   if(bestCoords.x == INF){
        bestCoords.x = -1;
        bestCoords.y = -1;
    }

    return bestCoords;
}


GridPoint CWorld::FindClosestShootingTarget(int shipNum)
{
    GridPoint myCoords = NeighborByDirection(myShips[shipNum].coords, myShips[shipNum].args[SHIP_ROTATION]);
    
    int estimate;
    int bestEstimate = INF;
    GridPoint bestCoords(INF, INF);

    for(auto currFoeShip : foeShips){
        for(int turnsPassed = 0; turnsPassed < 6; turnsPassed++) {

            GridPoint foeExpectedCoords = ApplySpeed(currFoeShip.second, turnsPassed);
            float dist = Distance(myCoords, foeExpectedCoords);
            int fireDist = 1 + round(dist/3);
            
            if(dist <= 10) {
                if(dist < 1)
                    estimate = INF;
                else
                    estimate = abs(turnsPassed - fireDist)*dist;

                // fprintf(stderr,"%i %i\nt %i f %i d %f est %i best %i\n", foeExpectedCoords.x, foeExpectedCoords.y, 
                //         turnsPassed, fireDist, dist, estimate, bestEstimate);

                if(estimate < bestEstimate) {
                    bestEstimate = estimate;
                    bestCoords = foeExpectedCoords;
                }
            }
        }
    }
    
    if(bestCoords.x == INF){
        bestCoords.x = -1;
        bestCoords.y = -1;
    }

    return bestCoords;
}


int CWorld::NavigationToTarget(Entity ship, GridPoint target)
{
    int cmd = CMD_MOVE;
    // GridPoint sideCoords = FindPath(ship, target);
    
    // GridPoint newCoords = ApplySpeed(ship, 0);
        
    // int destOrientation = DirectionByNeighbor(newCoords, sideCoords);
    
    // if(destOrientation == ship.args[SHIP_ROTATION]){
    //     // if(ship.args[SHIP_SPEED] < 2)
    //     //     cmd = CMD_FASTER;
    //     // else
    //     //     cmd = CMD_WAIT;
            
    //     if(ship.args[SHIP_SPEED])
    //         cmd = CMD_WAIT;
    //     else
    //         cmd = CMD_FASTER;
    // }
    // else {
    //     int rotationLeft = 0;
    //     for(int i = ship.args[SHIP_ROTATION]; i%6 != destOrientation%6; i++){
    //         rotationLeft++;
    //     }
        
    //     if(rotationLeft > 3)
    //         cmd = CMD_RIGHT;
    //     else
    //         cmd = CMD_LEFT;
            
    //     if(Distance(ship.coords, target) == 1 && ship.args[SHIP_SPEED] > 0)
    //         cmd = CMD_SLOWER;
        
    //     if(Distance(ship.coords, target) == 2 && ship.args[SHIP_SPEED] == 2)
    //         cmd = CMD_SLOWER;
    // }
    
    cmd = FindPathCmd(ship, target);
    
    return cmd;
}


GridPoint CWorld::FindPath(Entity ship, GridPoint target)
{
    fprintf(stderr, "From %i %i to %i %i\n", ship.coords.x, ship.coords.y, target.x, target.y);
    
    GridPoint newCoords = ApplySpeed(ship, 0);

    priority_queue <PointWrapper> pQueue;

    
    list <PointWrapper> store;
    
    pQueue.push(PointWrapper(newCoords, 0, ship.args[SHIP_ROTATION], 0, nullptr));
    //store.push_back(pQueue.top());
    //pQueue.push(PointWrapper(newCoords, 0, ship.args[SHIP_ROTATION], 0));
    
    
    while(!pQueue.empty()){
        PointWrapper curPoint = pQueue.top();
        
        store.push_back(curPoint);
        
        PointWrapper *parent = &store.back();
        
        pQueue.pop();
        
        //fprintf(stderr, "Check curPoint %i %i - %i\n", curPoint.coords.x, curPoint.coords.y, curPoint.keyValue);
        
        if(!curPoint.coords.isInsideMap()) continue;
        if(curPoint.coords == target) break;
                
        for(int i = 0; i < 6; i++){
            
            int newCost = curPoint.cost + 2;
            if(i==curPoint.direction)
                newCost--;
            
            
            GridPoint nbrCoords = NeighborByDirection(curPoint.coords, i);
            if(!nbrCoords.isInsideMap()) continue;
            
            // newCost += UpdateCost(nbrCoords);
            // newCost += UpdateCost(NeighborByDirection(nbrCoords, i));
            // newCost += UpdateCost(NeighborByDirection(nbrCoords, (i + 3)%6));
            
            // newCost += UpdateCost(NeighborByDirection(curPoint.coords, i));
            // newCost += UpdateCost(NeighborByDirection(curPoint.coords, (i + 3)%6));
            
            int priorityKey = (Distance(ship.coords, target) - Distance(nbrCoords, target))*2 - newCost;
            
            if(nbrCoords == target) priorityKey+=5;
            
            //fprintf(stderr, "nbr %i %i dir %i pri %i\n", nbrCoords.x, nbrCoords.y, i, priorityKey);
            
            pQueue.push(PointWrapper(nbrCoords, newCost, i, priorityKey, parent ));

            //pQueue.push(PointWrapper(nbrCoords, newCost, i, priorityKey, nullptr));
            
        }

    }

    //fprintf(stderr, "Go to %i %i\n", path.front().x, path.front().y);
    
    PointWrapper *firstStep = &store.back();

    while(1){
        if(firstStep->from == nullptr)
            break;
        if(firstStep->from == &store.front())
            break;

        firstStep = firstStep->from;
    }

    fprintf(stderr, "Go to %i %i\n\n", firstStep->coords.x, firstStep->coords.y);

    GridPoint retCoords = firstStep->coords;

    return retCoords;
    //return path.front();
}



///////////////////////////////////// CmdWrapper
struct CmdWrapper
{
    GridPoint coords;

    int keyValue;
    int cost;
    int direction;
    CmdWrapper *from;
    int cmd;
    int speed;
    int depth;
    
    CmdWrapper(int cmd, GridPoint coords, int cost, int direction, int speed, int keyValue, CmdWrapper *parent, int depth){
        this->cmd = cmd;
        this->coords = coords;
        this->keyValue = keyValue;
        this->cost = cost;
        this->direction = direction;
        this->from = parent;
        this->speed = speed;
        this->depth = depth;
    }
    
    bool operator > (const CmdWrapper& other) const{
        return (this->keyValue > other.keyValue);
    }
    
    bool operator < (const CmdWrapper& other) const{
        return (this->keyValue < other.keyValue);
    }
    
    bool operator==  (const CmdWrapper& other) const{
        if(this->keyValue == other.keyValue) return true; else return false;
    };
    
    bool operator!=  (const CmdWrapper& other) const{
        if(this->keyValue == other.keyValue) return false; else return true;
    };
};


enum {MOV_WAIT, MOV_LEFT, MOV_RIGHT, MOV_FASTER, MOV_SLOWER };
int mov_arr[] {MOV_WAIT, MOV_LEFT, MOV_RIGHT, MOV_FASTER, MOV_SLOWER};
string mov_str[] {"WAIT", "LEFT", "RIGHT", "FASTER", "SLOWER"};


double GridPoint::Angle(GridPoint targetPosition) {
    double dy = (targetPosition.y - this->y) * sqrt(3) / 2;
    double dx = targetPosition.x - this->x + ((this->y - targetPosition.y) & 1) * 0.5;
    double angle = -atan2(dy, dx) * 3 / M_PI;
    if (angle < 0) {
        angle += 6;
    } else if (angle >= 6) {
        angle -= 6;
    }
    return angle;
}
        

/*
int CWorld::Collisions(Entity ship)
{
    
}
*/

int CWorld::FindPathCmd(Entity& ship, GridPoint target)
{
    fprintf(stderr, "From %i %i to %i %i\n", ship.coords.x, ship.coords.y, target.x, target.y);
    
    //GridPoint newCoords = ApplySpeed(ship, 0);
    GridPoint newCoords = ship.coords;

    priority_queue <CmdWrapper> pQueue;

    currentShip = &ship;
    Entity barrel;
    int maxRum = 100;
    int barrelCount = 0;
    for(auto trgtBarrel : barrels){
        barrelCount++;
        if(trgtBarrel.second.coords.x == target.x && trgtBarrel.second.coords.y == target.y)
            barrel = trgtBarrel.second;
    }
    
    list <CmdWrapper> store;
    int initCost = 0;
    int initValue = 0;
    int initDir = ship.args[SHIP_ROTATION];
    int initSpeed = ship.args[SHIP_SPEED];
    int initDepth = 0;
    CmdWrapper *parent = nullptr;
    
    CBoard Board;
    
    int counter = 0;
    
    pQueue.push(CmdWrapper(CMD_WAIT, newCoords, initCost, initDir, initSpeed, initValue, parent, initDepth));
    
    //enum {CMD_MOVE, CMD_FIRE, CMD_MINE, CMD_LEFT, CMD_RIGHT, CMD_FASTER, CMD_SLOWER, CMD_WAIT};
    
    while(!pQueue.empty()){
        CmdWrapper curPoint = pQueue.top();
        store.push_back(curPoint);
        parent = &store.back();
        pQueue.pop();
        
        int inAngle = min (abs(curPoint.direction - curPoint.coords.Angle(target)), abs(curPoint.direction + 6 - curPoint.coords.Angle(target)));    

        counter++;
        
        //fprintf(stderr, "Check curPoint %i %i - %i\n", curPoint.coords.x, curPoint.coords.y, curPoint.keyValue);
        // fprintf(stderr, "%s\n", mov_str[curPoint.cmd].c_str());
        // fprintf(stderr, "Check curPoint %i %i dir %i speed %i angle %i pri %i depth %i\n", curPoint.coords.x, curPoint.coords.y, curPoint.direction, curPoint.speed, inAngle, curPoint.keyValue, curPoint.depth);
        
        if(!curPoint.coords.isInsideMap()) continue;
        
        GridPoint fCoords = NeighborByDirection(curPoint.coords, curPoint.direction);
        GridPoint bCoords = NeighborByDirection(curPoint.coords, (curPoint.direction + 3)%6);
        
        //if(curPoint.coords == target || fCoords == target || bCoords == target) break;
        
        int chkRum = abs( maxRum - (ship.args[SHIP_RUM] - curPoint.depth) + barrel.args[0]);
        //cerr <<"Chkrum = "<<chkRum<<endl;
        
        //if(curPoint.depth > 3 && store.front().cmd == MOV_WAIT && curPoint.cmd == MOV_WAIT) return MOV_WAIT;
        
        //if(counter >= 120) return MOV_WAIT;
        if(counter >= 120){
            break;
        }
        
        for(int cmd = 0; cmd < 5; cmd++){
            int newCost = curPoint.cost + 2;
            int direction = curPoint.direction;;
            int speed = curPoint.speed;
            
            GridPoint nextCoords;
            
            switch(cmd){
                case MOV_LEFT:
                    direction = (direction + 1)%6;
                    break;
                case MOV_RIGHT:
                    direction = (direction + 5)%6;
                    break;
                case MOV_FASTER:
                    if(curPoint.speed == 2)
                        continue;
                    speed++;
                    break;
                case MOV_SLOWER:
                    if(curPoint.speed == 0)
                        continue;
                    speed--;
                    break;
                case MOV_WAIT:
                default: 
                    ;
            }
            
            GridPoint chkCoords;
            nextCoords = curPoint.coords;
            for(int i = 0; i < speed; i++){
                chkCoords = ApplySpeed(nextCoords, curPoint.direction, 1, 0);
                if(!nextCoords.isInsideMap() ) {
                    speed = 0;
                    break;
                }
                else
                    nextCoords = chkCoords;
            }

                    
            newCost += ChkMov(nextCoords, curPoint.direction, curPoint.depth + 1);
            newCost += ChkMov(nextCoords, direction, curPoint.depth + 1);
            
            int angle = min (abs(direction - nextCoords.Angle(target)), abs(direction + 6 - nextCoords.Angle(target)));
            int priorityKey = (Distance(ship.coords, target) - Distance(nextCoords, target))*2 - angle*2 - newCost - chkRum;
            //if(nextCoords == target) priorityKey+=5;
            
            if(nextCoords == target && barrelCount <= myShipCount+1) priorityKey+=500;
            
            
            //fprintf(stderr, "cmd %s coord %i %i dir %i speed %i angle %i pri %i\n", mov_str[cmd].c_str(), nextCoords.x, nextCoords.y, direction, speed, angle, priorityKey);
            if(Board.grid[nextCoords.x + nextCoords.y*WIDTH].visited &&
                Board.grid[nextCoords.x + nextCoords.y*WIDTH].direction == direction &&
                Board.grid[nextCoords.x + nextCoords.y*WIDTH].speed == speed){
                    //cerr << "Visited\n";
                    continue;
            }
            
            pQueue.push(CmdWrapper(cmd, nextCoords, newCost, direction, speed, priorityKey, parent, parent->depth + 1));
            
            Board.grid[nextCoords.x + nextCoords.y*WIDTH].visited = 1;
            Board.grid[nextCoords.x + nextCoords.y*WIDTH].direction = direction;
            Board.grid[nextCoords.x + nextCoords.y*WIDTH].speed = speed;
        }
          
    }

    CmdWrapper *firstStep = &store.back();

    if(counter >= 120){
        CmdWrapper res = pQueue.top();
        firstStep = &res;
    }

    while(1){
        if(firstStep->from == nullptr)
            break;
        if(firstStep->from == &store.front())
            break;

        firstStep = firstStep->from;
    }

    //fprintf(stderr, "Go to %i %i\n\n", firstStep->coords.x, firstStep->coords.y);
    
    for(int i = 0; i < 5; i++)
        if(mov_arr[i] == firstStep->cmd)
            fprintf(stderr, "Command is %s\n\n", mov_str[i].c_str());

    return firstStep->cmd;
}


int CWorld::ChkMov(GridPoint coords, int dir, int depth)
{
    int newCost = 0;
        
    GridPoint fCoords = NeighborByDirection(coords, dir);
    GridPoint bCoords = NeighborByDirection(coords, (dir + 3)%6);
    
    if(coords.isInsideMap())
        newCost += UpdateCost(coords, depth);
    if(fCoords.isInsideMap())
        newCost += UpdateCost(fCoords, depth);
    if(bCoords.isInsideMap())
        newCost += UpdateCost(bCoords, depth);
    
    return newCost;
}


int CWorld::UpdateCost(GridPoint coords, int depth)
{
    int cost = 0;
    
    auto it = fullEntityMap.find(coords.x + WIDTH*coords.y);
    if (it != fullEntityMap.end())
        if(it->second.type=="BARREL")
            cost--;
        else if(it->second.type=="MINE")
            cost+=50;

    for(auto bomb : explosions){
        if(bomb.second.coords.x == coords.x && bomb.second.coords.y == coords.y){
            if(bomb.second.args[BOMB_TIMER] - depth == 0)
                cost+=50;
        }
    }
    
    for(auto ship : parts){
        if(ship.second.id == currentShip->id || ship.second.id == currentShip->id + BACK_OFSET || ship.second.id == currentShip->id + NOSE_OFSET)
            continue;
            
        if(ship.second.coords.x == coords.x && ship.second.coords.y == coords.y){
            cost+=50;
        }
    }
        
    return cost;
}


void CWorld::Output(vector <Order> &orders)
{
    for (auto it = orders.begin(); it < orders.end(); it++)    {
        cout << it->Out();
        cout << "\n";
    }
}


void CWorld::ClearAll()
{
    myShipsLast = myShips;
    
    myShips.clear();
    foeShips.clear();
    barrels.clear();
    explosions.clear();
    parts.clear();
    fullEntityMap.clear();
}



///////////////////////////////////// Main
int main() {
    int myShipCount;
    int entityCount;
    Entity entity;
    CWorld World;
        
    srand (time(NULL));
    
    while (1) {
        cin >> myShipCount; cin.ignore();
        cin >> entityCount; cin.ignore();
        
        for (int i = 0; i < entityCount; i++) {
            cin >> entity.id >> entity.type >> entity.coords.x >> entity.coords.y;
            
            for(int i = 0; i < ARGS_NUM; i++)
                cin >> entity.args[i];
            
            cin.ignore();
            
            World.UpdateEntity(entity);
        }
        
        World.MakeTurn(myShipCount);
    }
}
