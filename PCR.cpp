#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>

using namespace std;

#define ARGS_NUM        4


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
    
    Point (Point &other){
        this->x = other.x;
        this->y = other.y;
    }
};



///////////////////////////////////// Entity
struct Entity
{
    int id;
    string type;
    Point coords;
    int args[ARGS_NUM];
};



///////////////////////////////////// CWorld
class CWorld{
private:

public:
    CWorld() {};
    void UpdateEntity(Entity entity);
    void MakeTurn(int myShipCount);
};



void CWorld::UpdateEntity(Entity entity)
{

}


void CWorld::MakeTurn(int myShipCount)
{
    for (int i = 0; i < myShipCount; i++) {
            cout << "WAIT" << endl;
    }
}


///////////////////////////////////// Main
int main() {
    int skip = 0;
    int myShipCount;
    int entityCount;
    
    srand (time(NULL));
    
    CWorld World;
    
    while (1) {
        cin >> myShipCount; cin.ignore();
        cin >> entityCount; cin.ignore();
        
        for (int i = 0; i < entityCount; i++) {
            Entity entity;
            
            cin >> entity.id >> entity.type >> entity.coords.x >> entity.coords.y;
            
            for(int i = 0; i < ARGS_NUM; i++)
                cin >> entity.args[i];
            
            cin.ignore();
            
            World.UpdateEntity(entity);
        }
        
        World.MakeTurn(myShipCount);
    }
}
