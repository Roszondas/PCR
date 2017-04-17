#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    srand (time(NULL));
    
        
    int myX[3] {-1, -1, -1};
    int myY[3] {-1, -1, -1};
    
    int foeX[3] {-1, -1, -1};
    int foeY[3] {-1, -1, -1};
    
    int count[3] {0, 0, 0};
    
    
    while (1) 
    {
        int myShipCount; // the number of remaining ships
        cin >> myShipCount; cin.ignore();
        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        cin >> entityCount; cin.ignore();
       
        int X[3] {-1, -1, -1};
        int Y[3] {-1, -1, -1};
        int alive[3] {0, 0, 0};
        
        for (int i = 0; i < entityCount; i++) 
        {
            int entityId;
            string entityType;
            int x;
            int y;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            cin >> entityId >> entityType >> x >> y >> arg1 >> arg2 >> arg3 >> arg4; cin.ignore();

            cerr << entityId << " " << entityType;
            
            if(entityType == "SHIP")
            {
                cerr << " " << arg4;
                
                if(arg4 == 1)
                {
                    int id;
                    
                    if(entityId <= 1)
                        id = 0;
                    else if(entityId <= 3)
                        id = 1;
                    else if(entityId <= 5)
                        id = 2;
                        
                    myX[id] = x;
                    myY[id] = y;
                }
                else
                {
                    int id;
                    
                    if(entityId <= 1)
                        id = 0;
                    else if(entityId <= 3)
                        id = 1;
                    else if(entityId <= 5)
                        id = 2;
                        
                    alive[id] = 1;
                        
                    if(foeX[id] == x && foeY[id] == y)
                        count[id]++;
                    else
                        count[id] = 0;
                        
                    foeX[id] = x;
                    foeY[id] = y;
                }
                
            }
            
            
            
            if(entityType == "BARREL")
            {
                for (int i = 0; i < myShipCount; i++) 
                {
                    if(X[i] == -1) X[i] = 400;
                    if(Y[i] == -1) Y[i] = 400;
                    
                    if(abs(myX[i] - x) + abs(myY[i] - x) < abs(myX[i] - X[i]) + abs(myY[i] - Y[i]) )
                    {
                        X[i] = x;
                        Y[i] = y;
                    }
                }
                
            }
            
            if(entityType == "CANNONBALL")
                cerr << " " << arg2;
            
            cerr << endl;
        }
        
        
        
        for (int i = 0; i < myShipCount; i++) 
        {
            if(X[i] == -1) X[i] = rand() % 23;
            if(Y[i] == -1) Y[i] = rand() % 21;
            
            if(count[i] >= 2 && alive[i] > 0)
            {
                if(abs(myX[i] - foeX[i]) + abs(myY[i] - foeY[i]) < 10)
                    cout << "FIRE " << foeX[i] << " " << foeY[i] << endl;
                else
                    cout << "MOVE " << foeX[i] << " " << foeY[i] << endl;
            }
            else
                cout << "MOVE " << X[i] << " " << Y[i] << endl;
        }
    }
}
