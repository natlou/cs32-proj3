#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <stack>
#include <vector>
using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
    bool /* shotHit */, bool /* shipDestroyed */,
    int /* shipId */) {}

void AwfulPlayer::recordAttackByOpponent(Point /* p */) {}


//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

class HumanPlayer : public Player
{
public:
    HumanPlayer(string nm, const Game& g) : Player(nm, g) {}
    virtual ~HumanPlayer() {}
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
        bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
};

bool HumanPlayer::placeShips(Board& b)
{
    cout << name() << " must place " << game().nShips() << " ships." << endl;

    int i = -1;
    int j = -1;
    char dir_char;
    Direction dir;

    for (int k = 0; k < game().nShips(); k++)
    {
        b.display(false); 

        bool flag = true;

        while (flag) {
            cout << "Enter h or v for direction of " << game().shipName(k) << " (length " << game().shipLength(k) << "): ";
            cin >> dir_char;

            if (dir_char == 'h') {
                dir = HORIZONTAL;
                flag = false;
            }
            else if (dir_char == 'v') {
                dir = VERTICAL;
                flag = false;
            }
            else {
                cout << "Direction must be h or v." << endl; 
                cin.clear();  
                cin.ignore(10000, '\n');
            }
        }

        flag = true; 

        while (flag)
        {
            cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            if (!getLineWithTwoIntegers(i, j))
            {
                cout << "You must enter two integers." << endl;
            }
            else if (game().isValid(Point(i, j)))
            {
                if (dir == HORIZONTAL)
                {
                    if (!b.placeShip(Point(i,j), k, HORIZONTAL))
                    {
                        cout << "The ship can not be placed there." << endl;
                    }
                    else {
                        flag = false;
                    }
                  
                }
                else if (dir == VERTICAL)
                {
                    if (!b.placeShip(Point(i,j), k, VERTICAL))
                    {
                        cout << "The ship can not be placed there." << endl;
                    }
                    else {
                        flag = false;
                    }
                }
            }
        }
    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
    int i;
    int j;

    cout << "Enter the row and column to attack (e.g., 3 5): ";
    cin >> i >> j;
       
    return (Point(i, j));
}

void HumanPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
    bool /* shotHit */, bool /* shipDestroyed */,
    int /* shipId */) {}

void HumanPlayer::recordAttackByOpponent(Point /* p */) {}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player {
public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    bool DFS(Board& b, int counter);
    Point state2_initializer; //keeps track of the point that caused transition to state 2
    bool state1;
    vector<Point> attacked;
    
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g), state1(true) {}


bool MediocrePlayer::DFS(Board& b, int id)
{
    vector<Point> blacklist; // keeps track of the points that we dont want to visit 


    if (id == game().nShips()) // base case 
        return true;

    for (int i = 0; i < game().rows(); i++) { 

        for (int j = 0; j < game().cols(); j++) {

            Point p(i, j);

            bool blacklisted = false;

            for (int v = 0; v < blacklist.size(); v++) { // check if the current point is in the blacklist 

                if (p.r == blacklist[v].r && p.c == blacklist[v].c)
                    blacklisted = true;
            }

            if (blacklisted == false && b.placeShip(p, id, HORIZONTAL)) 
            { 
                if (DFS(b, id + 1) == false ) { // backtrack if next ship cannot be placed
                    b.unplaceShip(p, id, HORIZONTAL);
                    blacklist.push_back(p);  
                    i = 0;
                    j = 0;
                }
                else return true;
             
            }

            else if (blacklisted == false && b.placeShip(p, id, VERTICAL))
            {
                if (DFS(b, id + 1) == false) { 
                    b.unplaceShip(p, id, VERTICAL);
                    blacklist.push_back(p); 
                    i = 0;
                    j = 0;
                }
                else return true;
            }
        }
    }
    return false; 
}


bool MediocrePlayer::placeShips(Board& b) {
    for (int i = 0; i < 50; i++) { 
        b.block(); 

        if (DFS(b, 0)) 
        {
            b.unblock();
            return true;
        }

        b.unblock(); 
    }
    return false;
}


Point MediocrePlayer::recommendAttack() {

    Point attack;

    if (state1) { // state 1 basically just wants us to choose a random point to attack until we hit one 

        bool invalid_point = true;

        while (invalid_point)
        {
            invalid_point = false;
            attack = game().randomPoint(); // generate random point
            for (int i = 0; i < attacked.size(); i++) { //check if point is invalid
                if (attack.r == attacked[i].r && attack.c == attacked[i].c)
                    invalid_point = true;
            }
        }; 

        return attack;
    }

    else { // state 2

        bool invalid_point = true;
        bool initialized_point = false;

        while (invalid_point == true|| initialized_point == false); // check if the point is valid & not the initialized point 
        {
            invalid_point = false;
            initialized_point = false;
            attack = game().randomPoint(); // pick another random point 
            for (int i = 0; i < attacked.size(); i++) { 
                if (attack.r == attacked[i].r && attack.c == attacked[i].c)
                    invalid_point = true;
            }
            if ( ( ( ((state2_initializer.r - attack.r) <= 4) && ((state2_initializer.r - attack.r) >= -4)) && state2_initializer.c == attack.c)
                ||  ( ( ((state2_initializer.c - attack.c) <= 4) && ((state2_initializer.c - attack.c) >= -4)) && state2_initializer.r == attack.r)) {
                initialized_point = true; // check if point is the initalized point 
            }
        }
        return attack;
    }

    return attack;
}

void MediocrePlayer::recordAttackResult(Point p, bool valid_shot, bool shot_hit, bool ship_destroyed, int ship_id) {
    if (valid_shot == false) return; 

    attacked.push_back(p); //add p to list of attacked cells

    if (state1) { //state 1
        if (!shot_hit) //if shot didnt hit, stay in state 1
            return;
        else {
            if (ship_destroyed) //if ship destroyed, stay in state 1
                return;
            state2_initializer = p; // state 2 initializer
            return;
        }
    }

    if (!state1) { 
        int initialized = 0;
        int max = 0;

        // check if points have already been attacked  (rows)
        for (int i = -4; i < 5; i++) {
            Point test(state2_initializer.r + i, state2_initializer.c);
            if (game().isValid(test))
                max += 1;
            for (int v = 0; v < attacked.size(); v++) {
                if ((attacked[v].r == state2_initializer.r + i) && (attacked[v].c == state2_initializer.c))
                    initialized += 1;
            }
        }

        // columns
        for (int i = -4; i < 5; i++) {
            Point test(state2_initializer.r, state2_initializer.c + i);
            if (game().isValid(test))
                max += 1;
            for (int v = 0; v < attacked.size(); v++) {
                if ((attacked[v].c == state2_initializer.c + i) && (attacked[v].r == state2_initializer.r))
                    initialized += 1;
            }
        }

        if (initialized == max) { //if the points reach the max, then return to state 1 
            state1 = true;
            return;
        }


        if (shot_hit == false) 
            return;
        else {
            if (ship_destroyed) { 
                state1 = true; 
                return;
            }
            else { 
                return;
            }
        }

    }
}

void MediocrePlayer::recordAttackByOpponent(Point p) { ; }

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer GoodPlayer;

class GoodPlayer : public Player {
public:
    GoodPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    vector<Point> attacked;
    Point state2_initializer;
    bool isValid(Point p);
    bool state1;
    stack<Point> targets;
};

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm, g), state1(true) {}



bool GoodPlayer::placeShips(Board& b) { // randomly place ships in alternating directions
    Direction dir;

    for (int id = 0; id < game().nShips(); id++) { //place ships on game
        Point p;

        int counter = 0;

        if (id % 2 == 0) { 
            dir = HORIZONTAL;
        }
        else {
            dir = VERTICAL;
        }

        do {
            counter += 1 ;
            if (counter == 100) 
                return false;
            p = game().randomPoint();
        } while (!b.placeShip(p, id, dir)); //place random ship s.t. it fits on board
    }
    return true;
}


bool GoodPlayer::isValid(Point p) {
    if (!game().isValid(p)) return false;

    for (int i = 0; i < attacked.size(); i++) { 
        if (p.r == attacked[i].r && p.c == attacked[i].c) return false;
    }

    return true;
}

Point GoodPlayer::recommendAttack() { // saw on youtube, attack diagonals (checkboard) first (state 1), then if diagonal is successfully attacked, change to state 2 and go along that direction
   
    Point attack;

    if (state1 || targets.empty()) { 

        state1 = true; //if no more targets to attack in stack go back to state 1


        bool even = false; // basically checks for diagonals

        attack = game().randomPoint();

        if ((attack.r + attack.c) % 2 == 0)
        {
            even = true;
        }

        int counter = 0;

        while (isValid(attack) == false || even == false) // we only need to attack even points because every battleship is length 2 
        {
            counter += 1;
            attack = game().randomPoint();
            if ((attack.r + attack.c) % 2 == 0 || counter >= (((MAXROWS * MAXCOLS) / 2) - 1))
            {
                even = true;
            }
        }
        return attack; 
    }

    else { //state 2
        attack = targets.top(); //attack point on top of the stack
        targets.pop();
        return attack;
    }
    return attack;
}

void GoodPlayer::recordAttackResult(Point p, bool valid_shot, bool shot_hit, bool ship_destroyed, int ship_id) {

    if (!valid_shot)  return;

    attacked.push_back(p); // mark point as attacked

    if (state1) { 

        if (!shot_hit) return;

        else {
            if (ship_destroyed) { // stay in state 1 if ship destroyed
                return;
            }
            else { 
                state1 = false;

                // push potential attacks to the stack on all four direction 

                if (isValid(Point(p.r, p.c - 1))) targets.push(Point(p.r, p.c - 1)); // left 

                if (isValid(Point(p.r, p.c + 1))) targets.push(Point(p.r, p.c + 1)); // right

                if (isValid(Point(p.r - 1, p.c))) targets.push(Point(p.r - 1, p.c)); // up 

                if (isValid(Point(p.r + 1, p.c))) targets.push(Point(p.r + 1, p.c)); // down 

                return;

            }
        }
    }

    else { //state 2
        if (!shot_hit) {
            return;
        }
        else {
            if (ship_destroyed) { // once ship is destroyed we return to state 1 
                state1 = true; 
                while (!targets.empty()) { // clear targets
                    targets.pop();
                }
                return;
            }
            else { // we stay in state 2 and choose next direction based on last hit's performance

                if (state2_initializer.r == p.r) // we attack horizonally 
                { 
                    if (isValid(Point(p.r, p.c - 1))) targets.push(Point(p.r, p.c - 1)); // left 

                    if (isValid(Point(p.r, p.c + 1))) targets.push(Point(p.r, p.c + 1)); // right
                }

                else if (state2_initializer.c == p.c) // we attack vertically
                { 
                    if (isValid(Point(p.r - 1, p.c))) targets.push(Point(p.r - 1, p.c)); // up 

                    if (isValid(Point(p.r + 1, p.c))) targets.push(Point(p.r + 1, p.c)); // down 
                }
                return;
            }
        }

    }
}

void GoodPlayer::recordAttackByOpponent(Point p) { ; }



//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
