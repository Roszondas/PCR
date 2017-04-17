#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <map>

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
enum {OWNER_ME, OWNER_FOE};

string entity_name[ENTITY_TYPES_NUM] {"SHIP", "BARREL", "CANNONBALL", "MINE"};
int entity_type[ENTITY_TYPES_NUM] {SHIP, BARREL, EXPLOSION, MINE};


///////////////////////////////////// Point
struct Point {
    int x;
    int y;
    Point () {
        x = -1;
        y = -1;
    }
    
    Point (int x, int y){
        this->x = x;
        this->y = y;
    }
    
    Point (const Point &other){
        this->x = other.x;
        this->y = other.y;
    }
};



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
    Point coords;
    int args[ARGS_NUM];
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
    
    Order(int cmd, Point coords) {
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
    bool firstTurn = true;
    map <int, Entity> myShips;
    map <int, Entity> foeShips;
    map <int, Entity> barrels;
    
    void ShipInit(int myShipCount);
    void Output(vector <Order> &orders);
    Order DecisionMainSystem(int shipNum);
    Point FindClosestBarrel(int shipNum);
    void ClearAll();
    
public:
    CWorld() {};
    void UpdateEntity(Entity entity);
    void MakeTurn(int myShipCount);
};


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
    
    if(firstTurn){
        ShipInit(myShipCount);
        firstTurn = false;
    }
    
    for (int i = 0; i < myShipCount; i++) {
        orders.push_back(DecisionMainSystem(i));
        orders.back().Message(to_string(i));
    }
    
    this->Output(orders);
    
    ClearAll();
}


void CWorld::ShipInit(int myShipCount)
{
    
};

Order CWorld::DecisionMainSystem(int shipNum)
{
    int shipOrder;
    
    Point shipTargetCoords = FindClosestBarrel(shipNum);
    
    if(shipTargetCoords.x == -1){
        shipTargetCoords.x = rand() % WIDTH;
        shipTargetCoords.y = rand() % HEIGHT;
    }
    
    shipOrder = CMD_MOVE;
    
    return Order(shipOrder, shipTargetCoords);
}


Point CWorld::FindClosestBarrel(int shipNum)
{
    int myX = myShips[shipNum].coords.x;
    int myY = myShips[shipNum].coords.y;
    int bestX = INF;
    int bestY = INF;
    
    for(auto currBarrel : barrels){
        int barX = currBarrel.second.coords.x;
        int barY = currBarrel.second.coords.y;
        
        if(abs(myX - barX) + abs(myY - barY) < abs(myX - bestX) + abs(myY - bestY) )
        {
            bestX = barX;
            bestY = barY;
        }
    }
    
    if(bestX == INF){
        bestX = -1;
        bestY = -1;
    }
    
    return Point(bestX, bestY);
}

void CWorld::Output(vector <Order> &orders)
{
    for (auto it = orders.begin(); it < orders.end(); it++)
    {
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
