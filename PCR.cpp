#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <map>
#include <queue>
#include <chrono>

using namespace std;

#define INF        999999
#define INF_NEG    -999999

#define ARGS_NUM            4
#define ENTITY_TYPES_NUM    4
#define WIDTH               23
#define HEIGHT              21

#define _TIMER chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()

enum {CMD_MOVE, CMD_FIRE, CMD_MINE, CMD_LEFT, CMD_RIGHT, CMD_FASTER, CMD_SLOWER, CMD_WAIT};
enum {SHIP, BARREL, EXPLOSION, MINE};
enum {SHIP_ROTATION, SHIP_SPEED, SHIP_RUM, SHIP_OWNER};
enum {OWNER_FOE, OWNER_ME};

string entity_name[ENTITY_TYPES_NUM] {"SHIP", "BARREL", "CANNONBALL", "MINE"};
int entity_type[ENTITY_TYPES_NUM] {SHIP, BARREL, EXPLOSION, MINE};

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
struct EntityBase
{
    bool explosion = false;
    int args[2];
};

struct Entity : public EntityBase
{
    int id;
    string type;
    GridPoint coords;
    int args[ARGS_NUM];
};


///////////////////////////////////// PointWrapper
struct PointWrapper
{
    GridPoint coords;

    int keyValue;
    int cost;
    int direction;
    
    PointWrapper(GridPoint coords, int cost, int direction, int keyValue){
        this->coords = coords;
        this->keyValue = keyValue;
        this->cost = cost;
        this->direction = direction;
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
    map <int, Entity> myShips;
    map <int, Entity> foeShips;
    map <int, Entity> barrels;
    
    void Output(vector <Order> &orders);
    Order DecisionMainSystem(int shipNum);
    GridPoint FindClosestBarrel(int shipNum);
    GridPoint FindClosestShootingTarget(int shipNum);
    int NavigationToTarget(Entity ship, GridPoint target);
    GridPoint FindPath(Entity ship, GridPoint target);
    
    void ClearAll();
    
public:
    CWorld() {};
    void UpdateEntity(Entity entity);
    void MakeTurn(int myShipCount);
    static int Distance(CubePoint a, CubePoint b);
    static int Distance(GridPoint a, GridPoint b);
    static int DirectionByNeighbor(GridPoint coords, GridPoint Nbr);
    static GridPoint NeighborByDirection(GridPoint coords, int direction);
};



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
            if(entity.args[SHIP_OWNER] == OWNER_ME)
                container = &myShips;
            else
                container = &foeShips;
            break;
            
        case BARREL:
            container = &barrels;
            
        default:
            break;
    }
    
    if(container != nullptr)
        container->insert(pair<int, Entity> (entity.id, entity));
    
}


void CWorld::MakeTurn(int myShipCount)
{
    vector <Order> orders;

    auto T1 = _TIMER;
    
    for(auto &ships : myShips){
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
    
    if(shipTargetCoords.x == -1){
        shipTargetCoords.x = rand() % WIDTH;
        shipTargetCoords.y = rand() % HEIGHT;
    }
    
    shipOrder = NavigationToTarget(myShips[shipNum], shipTargetCoords);

    if(shipOrder == CMD_WAIT) {
        shipTargetCoords = FindClosestShootingTarget(shipNum);
        if(shipTargetCoords.x != -1)
            shipOrder = CMD_FIRE;
    }

    return Order(shipOrder, shipTargetCoords);
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
    
    GridPoint bestCoords(INF, INF);

    for(auto currFoeShip : foeShips){
        GridPoint foeExpectedCoords = currFoeShip.second.coords;
        
        for(int turnsPassed = 1; turnsPassed < 6; turnsPassed++) {
            for(int i = 0; i < currFoeShip.second.args[SHIP_SPEED]; i++){
                GridPoint chkCoords = NeighborByDirection(foeExpectedCoords, currFoeShip.second.args[SHIP_ROTATION]);
                if(chkCoords.isInsideMap())
                    foeExpectedCoords = chkCoords;
                else
                    break;
            }
            
            int dist = Distance(myCoords, foeExpectedCoords);
            int fireDist = 1 + dist/3;
            
            if(dist < 10) {
                if(fireDist <= turnsPassed + 1 && fireDist >= turnsPassed - 1)
                    bestCoords = foeExpectedCoords;
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
    GridPoint sideCoords = FindPath(ship, target);
    
    GridPoint newCoords = ship.coords;
    for(int i = 0; i < ship.args[SHIP_SPEED]; i++){
        GridPoint chkCoords = NeighborByDirection(newCoords, ship.args[SHIP_ROTATION]);
        if(chkCoords.isInsideMap())
            newCoords = chkCoords;
        else
            break;
    }
    
    int destOrientation = DirectionByNeighbor(newCoords, sideCoords);
    
    if(destOrientation == ship.args[SHIP_ROTATION]){
        // if(ship.args[SHIP_SPEED] < 2)
        //     cmd = CMD_FASTER;
        // else
        //     cmd = CMD_WAIT;
            
        if(ship.args[SHIP_SPEED])
            cmd = CMD_WAIT;
        else
            cmd = CMD_FASTER;
    }
    else {
        int rotationLeft = 0;
        for(int i = ship.args[SHIP_ROTATION]; i%6 != destOrientation%6; i++){
            rotationLeft++;
        }
        
        if(rotationLeft > 3)
            cmd = CMD_RIGHT;
        else
            cmd = CMD_LEFT;
            
        if(Distance(ship.coords, target) == 1 && ship.args[SHIP_SPEED] > 0)
            cmd = CMD_SLOWER;
        
        if(Distance(ship.coords, target) == 2 && ship.args[SHIP_SPEED] == 2)
            cmd = CMD_SLOWER;
    }
        
    return cmd;
}


GridPoint CWorld::FindPath(Entity ship, GridPoint target)
{
    GridPoint newCoords = ship.coords;
    int direction = ship.args[SHIP_ROTATION];
    
    //fprintf(stderr, "From %i %i to %i %i\n\n", newCoords.x, newCoords.y, target.x, target.y);
    
    for(int i = 0; i < ship.args[SHIP_SPEED]; i++){
        GridPoint chkCoords = NeighborByDirection(newCoords, direction);
        if(chkCoords.isInsideMap())
            newCoords = chkCoords;
        else
            break;
    }
    
    priority_queue <PointWrapper> pQueue;
    queue <GridPoint> path;
    
    pQueue.push(PointWrapper(newCoords, 0, direction, 0));
    
    while(!pQueue.empty()){
        PointWrapper curPoint = pQueue.top();
        path.push(curPoint.coords);
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
            
            int priorityKey = (Distance(ship.coords, target) - Distance(nbrCoords, target))*2 - newCost;
            
            if(nbrCoords == target) priorityKey+=5;
            
            //fprintf(stderr, "nbr %i %i pri %i\n", nbrCoords.x, nbrCoords.y, priorityKey);
            
            pQueue.push(PointWrapper(nbrCoords, newCost, i, priorityKey));
        }

    }
    
    path.pop();
    
    //fprintf(stderr, "Go to %i %i\n", path.front().x, path.front().y);
    
    return path.front();
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
    myShips.clear();
    foeShips.clear();
    barrels.clear();
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
