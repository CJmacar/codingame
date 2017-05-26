nclude <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iterator>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
namespace Entities
{
    class Base
    {
        public:
        string getPosition() const { return to_string(m_x) + " " + to_string(m_y); }
        int getX() const { return m_x; }
        int getY() const { return m_y; }
        int getId() const { return m_id; }
        vector<int> getDistance() const { return m_distance; }
        void setDistance(int d) { m_distance.push_back(d);}
        
        bool operator<(const Base& rhs) const
        {
            return m_distance[0] < rhs.getDistance()[0];
        }
        
        protected:
        Base(const int& id, const int& x, const int& y, int distance = 0) : m_id(id), m_x(x), m_y(y), m_distance(distance) {}
        Base() = default;
        
        vector<int> m_distance;
        int m_id;
        int m_x;
        int m_y;
    };
        
    class Ship : public Base
    {
        public:
        Ship(const int& id, 
             const int& x, 
             const int& y, 
             const int& rotation, 
             const int& speed, 
             const int& stock,
             const bool& mine) : Base(id, x, y), m_rotation(rotation), m_speed(speed), m_stock(stock), m_mine(mine) {}
        Ship() = default;
        
        bool operator<(const Ship& rhs) const
        {
            return m_id < rhs.getId();
        }
        
        int getSpeed() const { return m_speed; } 
        int getRotation() const { return m_rotation; }
        int getStock() const { return m_stock; }
        
        private:
        int m_rotation;
        int m_speed;
        int m_stock;
        bool m_mine;
    };
    
    class Barrel : public Base
    {
        public:
        Barrel(const int& id, 
               const int& x,
               const int& y,
               int distance,
               const int& amount) : Base(id, x, y, distance), m_amount(amount) {}
        Barrel() = default;
       
        bool operator==(const Barrel& rhs) const
        {
            return m_x == rhs.getX() && m_y == rhs.getY();
        }
        
        bool operator!=(const Barrel& rhs) const
        {
            return !(*this==rhs);
        }
        
        int getAmount() const { return m_amount; }
    
        private:
        int m_amount;    
        int m_rank;
    };
    
    class Mine : public Base
    {
        public:
        Mine(int id, int x, int y, int distance = 0) : Base(id, x, y, distance) {}
    };
 }

namespace Helpers
{
    enum class entityType
    {
        SHIP,
        BARREL,
        MINE
    };
    
    entityType string2type(const string & str)
    {
        if(str == "BARREL")
        {
            return entityType::BARREL;
        }
        else if(str == "SHIP")
        {
            return entityType::SHIP;
        }
        else if(str == "MINE")
        {
            return entityType::MINE;
        }
        else
        {
       //     cerr << "Unknown entity type: " << str << endl;
        }
    }
    
    template<typename T>
    bool wrongDirection(const Entities::Ship& ship, const T object)
    {
        int nextX = 0, nextY = 0;

        switch(ship.getRotation())
        {
            case 0:
                nextX = 1;
                break;
            case 1:
                nextY = -1;
                if(ship.getX()%2) nextX = 1;
                break;
            case 2:
                nextY = -1;
                if(!(ship.getX()%2)) nextX = -1;
                break;
            case 3:
                nextX = -1;
                break;
            case 4:
                nextY = 1;
                if(!(ship.getX()%2)) nextX = -1;
                break;
            case 5:
                nextY = 1;
                if(ship.getX()%2) nextX = 1;
                break;
        }
        
        if(ship.getX() > object.getX() && nextX > 0)
            return true;
        else if(ship.getX() < object.getX() && nextX < 0)
            return true;
        else if(ship.getY() > object.getY() && nextY > 0)
            return true;
        else if(ship.getY() < object.getY() && nextY < 0)
            return true;
            
        return false;
    }
    
    template<typename T>
    bool inDirection(const Entities::Ship& ship, const T object)
    {
        int nextX = 0, nextY = 0;

        switch(ship.getRotation())
        {
            case 0:
                nextX = 1;
                break;
            case 1:
                nextY = -1;
                if(ship.getX()%2) nextX = 1;
                break;
            case 2:
                nextY = -1;
                if(!(ship.getX()%2)) nextX = -1;
                break;
            case 3:
                nextX = -1;
                break;
            case 4:
                nextY = 1;
                if(!(ship.getX()%2)) nextX = -1;
                break;
            case 5:
                nextY = 1;
                if(ship.getX()%2) nextX = 1;
                break;
        }
        
        if((ship.getX() < object.getX()) && nextX > 0 && 
           (ship.getY() < object.getY()) && nextY > 0)
           return true;
        else if((ship.getX() > object.getX()) && nextX < 0 && 
                (ship.getY() > object.getY()) && nextY < 0)
           return true;
            
        return false;
    }
    
    int calculateDistance(int aX, int aY, int bX, int bY)
    {
        return max(abs(aX-bX), abs(aY-bY));
    }
}
 
int main()
{
    vector<Entities::Barrel> lastTarget;
    int count = 0;
    bool shotLast = false;
    vector<bool> evasive;
    vector<int> turnAround;
    // game loop
    while (1) {
        vector<bool> action;
        
        int myShipCount; // the number of remaining ships
        cin >> myShipCount; cin.ignore();
        
        for(int i = 0; i < myShipCount; ++i)
        {
            action.push_back(false);
            lastTarget.push_back(Entities::Barrel());
            evasive.push_back(false);
            turnAround.push_back(0);
        }
        
        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        cin >> entityCount; cin.ignore();
        
        vector<Entities::Barrel> barrels;
        vector<Entities::Ship> myShips;
        vector<Entities::Ship> hostileShips;
        vector<Entities::Mine> mines;
        
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            string entityType;
            int x;
            int y;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            cin >> entityId >> entityType >> x >> y >> arg1 >> arg2 >> arg3 >> arg4; cin.ignore();
            
            switch(Helpers::string2type(entityType))
            {
                case Helpers::entityType::SHIP:
                    if(arg4 == 1)
                        myShips.emplace_back(entityId, x, y, arg1, arg2, arg3, true);
                    else
                        hostileShips.emplace_back(entityId, x, y, arg1, arg2, arg3, false);
                    break;
                case Helpers::entityType::BARREL:
                    barrels.emplace_back(Entities::Barrel(entityId, x, y, 0, arg1));;
                    break;
                case Helpers::entityType::MINE:
                    mines.emplace_back(entityId, x, y);
                default:
                break;
            }                
        }
        
        sort(myShips.begin(),myShips.end());
        
        for_each(mines.begin(), mines.end(), [&](Entities::Mine & aMine)
        {
            for (int i = 0; i < myShipCount; i++)
            {
                int d = Helpers::calculateDistance(aMine.getX(), aMine.getY(), myShips[i].getX(), myShips[i].getY());
                aMine.setDistance(d);
            }    
        });
        
        if(!mines.empty()){
            for (int i = 0; i < myShipCount; i++)
            {
               auto closeMine = find_if(mines.cbegin(), mines.cend(), [&](const Entities::Mine& a){  
                   return a.getDistance()[i] < 4 && Helpers::inDirection(myShips[i], a);});
          
               if(closeMine != mines.cend() && myShips[i].getSpeed() > 1)
               {
                   cout << "FIRE " + closeMine->getPosition() << endl;
                   action[i] = true;
               }
               
            }
        }

        for_each(barrels.begin(), barrels.end(), [&](Entities::Barrel & aBarrel)
        {
            for (int i = 0; i < myShipCount; i++)
            {
                int d = Helpers::calculateDistance(aBarrel.getX(), aBarrel.getY(), myShips[i].getX(), myShips[i].getY());
                 aBarrel.setDistance(d);
            }
        });
        
        for_each(hostileShips.begin(), hostileShips.end(), [&](Entities::Ship & aShip)
        {
            for (int i = 0; i < myShipCount; i++)
            {
                int d = Helpers::calculateDistance(aShip.getX(), aShip.getY(), myShips[i].getX(), myShips[i].getY());
                 aShip.setDistance(d);
            }
        });
        
        
       /* 
        for(auto i : barrels)
        {
            cerr << "Pos= " << i.getPosition() << " Amount= " << i.getAmount() << endl;
        }
        cerr << endl;*/
    
        if(!barrels.empty())
        {            
            for (int i = 0; i < myShipCount; i++) {
                //bool wrongDirection = false;
                if(!action[i]){
                    vector<int> distances;
                    for(auto aBarrel: barrels)
                    {
                        distances.push_back(aBarrel.getDistance()[i]);
                    }
                    
                    auto minDistance = min_element(distances.cbegin(), distances.cend());
     
                    auto found = find_if(barrels.begin(), barrels.end(), [&](const Entities::Barrel& a){
                        return a.getDistance()[i] == *minDistance;
                    });
                    
                    vector<int> amounts;
                    for(auto aBarrel: barrels)
                    {
                        amounts.push_back(aBarrel.getAmount());
                    }
                    
                    auto maxAmount = max_element(amounts.cbegin(), amounts.cend());
                    
                    if(myShips[i].getStock() < 20)
                    {
                        found = find_if(barrels.begin(), barrels.end(), [&](const Entities::Barrel& a){
                    	    return a.getAmount() == *maxAmount;
                        });
		    }
                    
                    auto it = find(lastTarget.cbegin(), lastTarget.cend(), *found);
                    
                    cerr << "TR " << distance(lastTarget.cbegin(), it) << " : " << i << endl;
                    
                   if( (it!=lastTarget.cend()) && (distance(lastTarget.cbegin(), it) != i && barrels.size() > 1) ){
                        advance(found,1);
                        cerr << "same target" << endl;
                        if(found == barrels.end()){
                            advance(found, -2);
                            cerr << "last target" << endl;
                        }
                    }   
                    
                    if( Helpers::wrongDirection<Entities::Barrel>(myShips[i], *found))
                        turnAround[i]++;
                    
                    if(*found != lastTarget[i] || myShips[i].getSpeed() < 1 ||  turnAround[i] > 2)
                    {
                        if(evasive[i])
                        {
                            set<int> distances;
                            for(auto && aShip: hostileShips)
                            {
                                distances.emplace(aShip.getDistance()[i]);
                            }
                            
                            auto found = find_if(hostileShips.begin(), hostileShips.end(), [&](const Entities::Ship& a){
                                return a.getDistance()[i] == *distances.begin();
                            });
                            
                            cout << "FIRE " + found->getPosition() << endl;
                            
                            shotLast = true;
                            ++count;
                            evasive[i] = false;
                            cerr << "EVASIVE!" << endl;
                        }
                        else 
                        {
                            if(myShips[i].getSpeed() < 1){
                                evasive[i] = true;
                                cerr << "setting evasive" << endl;
                            }

                            cout << "MOVE " + found->getPosition() << endl; // Any valid action, such as "WAIT" or "MOVE x y"
                            lastTarget[i] = *found;
                            turnAround[i] = 0;
                        }
                    }
                    else
                    {
                        set<int> distances;
                        for(auto && aShip: hostileShips)
                        {
                            distances.emplace(aShip.getDistance()[i]);
                        }
                        
                        auto found = find_if(hostileShips.begin(), hostileShips.end(), [&](const Entities::Ship& a){
                            return a.getDistance()[i] == *distances.begin();
                        });
                        cout << "FIRE " + found->getPosition() << endl;
                    }
                }
            }
        }
        else
        {
            cerr << "empty" << endl;
            for (int i = 0; i < myShipCount; i++)
            {
                if(!action[i]){
                    if(myShips[i].getSpeed() > 0 && !shotLast){
                        set<int> distances;
                        
                        for(auto && aShip: hostileShips)
                        {
                            distances.emplace(aShip.getDistance()[i]);
                        }
                        
                        auto found = find_if(hostileShips.begin(), hostileShips.end(), [&](const Entities::Ship& a){
                            return a.getDistance()[i] == *distances.begin();
                        });
                        
                        cout << "FIRE " + found->getPosition() << endl;
                        
                        shotLast = true;
                        }
                    else
                    {
                        cout << "MOVE " + to_string(count) + " " + to_string(count) << endl;
                        shotLast = false;
                        ++count;
                        if(count > 19)
                            count = 0;
                    }
                }
            }
        }
    }
}
