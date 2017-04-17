#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>

using namespace std;

#define ARGS_NUM            4
#define ENTITY_TYPES_NUM    4


enum {CMD_MOVE, CMD_FIRE, CMD_MINE, CMD_LEFT, CMD_RIGHT, CMD_FASTER, CMD_SLOWER, CMD_WAIT};
enum {SHIP, BARREL, EXPLOSION, MINE};
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
    void Output(vector <Order> &orders);
    Order DecisionMainSystem(int shipNum);
    Point FindClosestBarrel(int shipNum);
    
public:
    CWorld() {};
    void UpdateEntity(Entity entity);
    void MakeTurn(int myShipCount);
};


void CWorld::UpdateEntity(Entity entity)
{
    int type = -1;
    
    for(int i = 0; i < ENTITY_TYPES_NUM; i++) {
        if(entity.type == entity_name[i]){
            type = i;
            break;
        }
    }
    
    switch(type){
        case SHIP :   break;
        default   :   break;
    }
    
}


void CWorld::MakeTurn(int myShipCount)
{
    vector <Order> orders;
        
    for (int i = 0; i < myShipCount; i++) {
        orders.push_back(DecisionMainSystem(i));
        orders.back().Message(to_string(i));
    }
    
    this->Output(orders);
}


Order CWorld::DecisionMainSystem(int shipNum)
{
    int shipOrder;
    
    Point shipTargetCoords = FindClosestBarrel(shipNum);
    
    shipOrder = CMD_MOVE;
    
    return Order(shipOrder, shipTargetCoords);
}


Point CWorld::FindClosestBarrel(int shipNum)
{
    return Point(rand() % 23, rand() % 21);
}

void CWorld::Output(vector <Order> &orders)
{
    for (auto it = orders.begin(); it < orders.end(); it++)
    {
        cout << it->Out();
        cout << "\n";
    }
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
