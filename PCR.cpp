#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    srand (time(NULL));
    
        
    int myX = -1, myY = -1;
    int foeX = -1, foeY = -1;
    int count = 0;
    
    while (1) 
    {
        int myShipCount; // the number of remaining ships
        cin >> myShipCount; cin.ignore();
        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        cin >> entityCount; cin.ignore();
       
        int X = -1, Y = -1;
        
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

            cerr << entityId << " " << entityType << endl;
            
            if(entityType == "SHIP")
            {
                if(arg4 == 1)
                {
                    myX = x;
                    myY = y;
                }
                else
                {
                    if(foeX == x && foeY == y)
                        count++;
                    else
                        count = 0;
                        
                    foeX = x;
                    foeY = y;
                }
                
            }
            
            if(entityType == "BARREL")
            {
                if(X == -1) X = 400;
                if(Y == -1) Y = 400;
                
                if(abs(myX - x) + abs(myY - x) < abs(myX - X) + abs(myY - Y) )
                {
                    X = x;
                    Y = y;
                }
                
            }
        }
        
        if(X == -1) X = rand() % 23;
        if(Y == -1) Y = rand() % 21;
        
        for (int i = 0; i < myShipCount; i++) 
        {
            if(count >= 2)
            {
                if(abs(myX - foeX) + abs(myY - foeY) < 10)
                    cout << "FIRE " << foeX << " " << foeY << endl;
                else
                    cout << "MOVE " << foeX << " " << foeY << endl;
            }
            else
                cout << "MOVE " << X << " " << Y << endl;
        }
    }
}
