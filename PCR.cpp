#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <map>
#include <queue>

using namespace std;

#define INF        999999
#define INF_NEG    -999999

#define ARGS_NUM            4
#define ENTITY_TYPES_NUM    4
#define WIDTH               23
#define HEIGHT              21


enum {CMD_MOVE, CMD_FIRE, CMD_MINE, CMD_LEFT, CMD_RIGHT, CMD_FASTER, CMD_SLOWER, CMD_WAIT};
enum {SHIP, BARREL, EXPLOSION, MINE};
enum {SHIP_ROTATION, SHIP_SPEED, SHIP_RUM, SHIP_OWNER};
enum {OWNER_FOE, OWNER_ME};

string entity_name[ENTITY_TYPES_NUM] {"SHIP", "BARREL", "CANNONBALL", "MINE"};
int entity_type[ENTITY_TYPES_NUM] {SHIP, BARREL, EXPLOSION, MINE};

int DIRECTIONS_EVEN[][2] { { 1, 0 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 } };
int DIRECTIONS_ODD[][2]  { { 1, 0 }, { 1, -1 }, { 0, -1 },  { -1, 0 }, { 0, 1 },  { 1, 1 } };

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


//////////////////////////////////////////////////////////

struct PointWrapper
{
    GridPoint coords;
    int keyValue;
    int cost;
    int orientation;
    
    PointWrapper(GridPoint coords, int cost, int orientation, int keyValue){
        this->coords = coords;
        this->keyValue = keyValue;
        this->cost = cost;
        this->orientation = orientation;
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


GridPoint Nbrs(GridPoint coords, int orientation)
{
    if (coords.y % 2 == 1) {
            coords.y += DIRECTIONS_ODD[orientation][1];
            coords.x += DIRECTIONS_ODD[orientation][0];
        } else {
            coords.y += DIRECTIONS_EVEN[orientation][1];
            coords.x += DIRECTIONS_EVEN[orientation][0];
        }
    
    return coords;
};


int Orient(GridPoint coords, GridPoint Nbr)
{
    int orientation;
    
    if (coords.y % 2 == 1) {
        for(int i = 0; i < 6; i++){
            if(Nbr.y == coords.y + DIRECTIONS_ODD[i][1] && Nbr.x == coords.x + DIRECTIONS_ODD[i][0]){
                orientation = i;
                break;
            }
        }
    }
    else {
        for(int i = 0; i < 6; i++){
            if(Nbr.y == coords.y + DIRECTIONS_EVEN[i][1] && Nbr.x == coords.x + DIRECTIONS_EVEN[i][0]){
                orientation = i;
                break;
            }
        }
    }
    
    return orientation;
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
    int NavigationToTarget(Entity ship, GridPoint target);
    GridPoint FindPath(Entity ship, GridPoint target);
    
    void ClearAll();
    
public:
    CWorld() {};
    void UpdateEntity(Entity entity);
    void MakeTurn(int myShipCount);
    static int Distance(CubePoint a, CubePoint b);
    static int Distance(GridPoint a, GridPoint b);
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

    for(auto &ships : myShips){
        orders.push_back(DecisionMainSystem(ships.first));
        orders.back().Message(to_string(ships.first));
    }

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

    //shipTargetCoords = FindPath(myShips[shipNum], shipTargetCoords);

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


int CWorld::NavigationToTarget(Entity ship, GridPoint target)
{
    int cmd = CMD_MOVE;
    GridPoint sideCoords = FindPath(ship, target);
    
    GridPoint newCoords = ship.coords;
    for(int i = 0; i < ship.args[SHIP_SPEED]; i++){
        GridPoint chkCoords = Nbrs(newCoords, ship.args[SHIP_ROTATION]);
        if(chkCoords.isInsideMap())
            newCoords = chkCoords;
        else
            break;
    }
    
    int destOrientation = Orient(newCoords, sideCoords);
    
    if(destOrientation == ship.args[SHIP_ROTATION]){
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
            
    }
        
    return cmd;
}




GridPoint CWorld::FindPath(Entity ship, GridPoint target)
{
    GridPoint newCoords = ship.coords;
    int orientation = ship.args[SHIP_ROTATION];
    
    fprintf(stderr, "From %i %i to %i %i\n\n", newCoords.x, newCoords.y, target.x, target.y);
    
    for(int i = 0; i < ship.args[SHIP_SPEED]; i++){
        GridPoint chkCoords = Nbrs(newCoords, orientation);
        if(chkCoords.isInsideMap())
            newCoords = chkCoords;
        else
            break;
    }
    
    priority_queue <PointWrapper> pQueue;
    queue <GridPoint> path;
    
    pQueue.push(PointWrapper(newCoords, 0, orientation, 0));
    
    while(!pQueue.empty()){
        PointWrapper curPoint = pQueue.top();
        path.push(curPoint.coords);
        pQueue.pop();
        
        //fprintf(stderr, "Check curPoint %i %i - %i\n", curPoint.coords.x, curPoint.coords.y, curPoint.keyValue);
        
        if(!curPoint.coords.isInsideMap()) continue;
        if(curPoint.coords == target) break;
                
        for(int i = 0; i < 6; i++){
            
            int newCost = curPoint.cost + 1;
            if(i==curPoint.orientation)
                newCost--;
            
            GridPoint nbrCoords = Nbrs(curPoint.coords, i);
            if(!nbrCoords.isInsideMap()) continue;
            
            int priorityKey = Distance(ship.coords, target) - Distance(nbrCoords, target) - newCost;
            
            //fprintf(stderr, "nbr %i %i pri %i\n", nbrCoords.x, nbrCoords.y, priorityKey);
            
            pQueue.push(PointWrapper(nbrCoords, newCost, i, priorityKey));
        }

    }
    
    path.pop();
    
    fprintf(stderr, "Go to %i %i", path.front().x, path.front().y);
    
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
