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
    int i = -1;
    int j = -1;
    char dir_char;
    Direction dir;

    for (int k = 0; k < game().nShips(); k++)
    {
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
            }
        }

        flag = true; 

        while (flag)
        {
            cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            cin >> i >> j;

            
            if (game().isValid(Point(i, j))) 
                flag = false; 
        }

        if (!b.placeShip(Point(i, j), k, dir))
            return false;
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

class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string nm, const Game& g) : Player(nm, g), m_lastCellAttacked(0, 0) { state1 = true; }
    virtual ~MediocrePlayer() {}
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
        bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    bool DFS(Board& b, Point p, int id, stack<Point> current_ships);
    Point m_lastCellAttacked;
    vector<Point> attackedCells;
    vector<Point> limitedCells;
    vector<Point> attackedLimitedCells;
    Point state2_point;
    bool state1;
};

// TODO:  You need to replace this with a real class declaration and
//        implementation.

bool MediocrePlayer::DFS(Board& b, Point p, int id, stack<Point> current_ships)
{
    if (id == game().nShips()) return true; //return true if all the ships have been placed 

    if (p.c == game().cols())
    {
        return DFS(b, Point(p.r + 1, 0), id, current_ships); // if the column is past the last one, it goes to the next row
    }

    if (p.r == game().rows()) // if the last row was passed, then it does something 
    {
        if (id == 0) {
            return false; // if it backtracks to the first ship, or the first ship cannot find a place to be placed anymore, then it returns false 
        }
        
        Point backtrack = current_ships.top(); // backtracks

        current_ships.pop(); 

        return DFS(b, Point(backtrack.r, backtrack.c + 1), id - 1, current_ships); // tries to replace the last ship placed (in the stack)

    }

    if (b.placeShip(p, id, HORIZONTAL)) // tries horizontal placement 
    {
        current_ships.push(p);
        DFS(b, Point(0, 0), id + 1, current_ships);
    } 
    else if (b.placeShip(p, id, VERTICAL))
    {
        current_ships.push(p);
        DFS(b, Point(0, 0), id + 1, current_ships);
    }
    
    return DFS(b, Point(p.r, p.c + 1), id, current_ships); // next column 

}

bool MediocrePlayer::placeShips(Board& b)
{


    for (int i = 0; i < 50; i++)
    {
        b.block();

        stack<Point> empty_stack;

        if (DFS(b, Point(0, 0), 0, empty_stack))
        {
            return (true);
        }

        b.unblock();
    }
    
    return (false);
}

void MediocrePlayer::recordAttackByOpponent(Point /* p */) {} // does nothing 

Point MediocrePlayer::recommendAttack() 
{
    if (state1)
    {
        m_lastCellAttacked = game().randomPoint();

        if (attackedCells.size() > 0)
        {
            bool unique = false;
            while (!unique)
            {
                m_lastCellAttacked = game().randomPoint(); 
                for (int c = 0; c < attackedCells.size(); c++)
                {
                    if (m_lastCellAttacked.r == attackedCells[c].r && m_lastCellAttacked.c == attackedCells[c].c)
                    {
                        unique = false;
                        break;
                    }
                    else
                    {
                        unique = true;

                    }
                }
            }
        }
    }
    if (!state1) // currently in state2 
    {
        if (limitedCells.size() > 0)
        {
            int count = 0;
            for (int c = 0; c < attackedLimitedCells.size(); c++)
            {
                for (int d = 0; d < limitedCells.size(); d++)
                {
                    if (attackedLimitedCells[c].r == limitedCells[d].r && attackedLimitedCells[c].c == limitedCells[d].r)
                    {
                        count++;
                    }
                }
            }
            if (count >= limitedCells.size())
            {
                state1 = true;
                return recommendAttack(); //recursively attack
            }

        }

        if (attackedCells.size() > 0)
        {

            bool unique = false;
            bool valid = true;
            while (!unique)
            {
                valid = true;
                m_lastCellAttacked = game().randomPoint();

                for (int c = 0; c < limitedCells.size(); c++)
                {
                 
                    if (m_lastCellAttacked.r == limitedCells[c].r && m_lastCellAttacked.c == limitedCells[c].c)
                    {
 
                        if (attackedLimitedCells.size() > 0)
                        {
                            for (int d = 0; d < attackedLimitedCells.size(); d++)
                            {
                                if (attackedLimitedCells[d].c == m_lastCellAttacked.c && attackedLimitedCells[d].r == m_lastCellAttacked.r)
                                {
                                    valid = false;
                                }
                            }
                        }
                        if (valid)
                        {
                            attackedLimitedCells.push_back(m_lastCellAttacked);
                            unique = true;
                            break; 
                        }
                    }
                    else
                    {
                        unique = false;
                    }
                }
            }
        }

    }
    return m_lastCellAttacked;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
    bool shipDestroyed, int shipId) 
{
    attackedCells.push_back(p); //so i dont hit again
    bool justSwitched = false;
    if (state1)
    {
        if (!shotHit)
        {
            state1 = true;
        }
        if (shotHit && shipDestroyed)
        {
            state1 = true;
        }
        else if (shotHit && !shipDestroyed)
        {
            state1 = false;
            justSwitched = true;
        }
    }
    if (!state1)
    {
        /*In state 2, let (r,c) be the position that was hit that caused the transition from state
         1 to state 2. In state 2, recommendAttack returns a randomly chosen position
         drawn from a limited set: the set of valid positions no more than 4 steps away
         from (r,c) either horizontally or vertically, that have not been chosen before. This
         set defines a cross around (r,c). For example, assume that in state 1, a shot was
         fired on position (5,3) that hit a ship, but did not destroy it, resulting in:
         */
        if (justSwitched)
        {
            // cout<<"YOU FUKING TRIGGERED ME"<<endl;

            if (limitedCells.size() > 0)
            {
                for (int c = 0; c < limitedCells.size(); c++)
                {
                    limitedCells.pop_back();
                }
            }

            state2_point = p; //4 4   0  0
            int rS2Left = state2_point.r - 4; //4 0    - 4 0    (my row left so -4 off grid
            int rS2Right = state2_point.r + 4;  //4 8     8  0
            int cS2Top = state2_point.c - 4; //0 4     -4   4
            int cS2Bot = state2_point.c + 4; // 8 4

            attackedLimitedCells.push_back(state2_point);


            //run a check to see that if these values are also in the point of attacked cells. if they are, then dont even bother trying to push it in as a coordinate. also, check if the coordinate to push in is a valid one to begin with. if this is implemented right, the above wouldn't have to worry about iterators at all.
            for (int c = rS2Left; c <= rS2Right; c++) //-4 -3 -2 -1 0 1 2 3
            {
                bool canpush = true;
                for (int d = 0; d < attackedCells.size(); d++)
                {//if attacked cells
             //1 0
                    if (attackedCells[d].r == c && attackedCells[d].c == state2_point.c)
                    {
                        canpush = false;
                    }
                    else
                    {

                    }
                }
                Point tryingtoPush;
                tryingtoPush.r = c;
                tryingtoPush.c = state2_point.c;
                if (!game().isValid(tryingtoPush))
                {
                    canpush = false;
                }

                if (canpush)
                {
                    limitedCells.push_back(Point(c, state2_point.c));//coordinates for left to right
                }
            }
            for (int c = cS2Top; c <= cS2Bot; c++)
            {
                bool canpush = true;
                for (int d = 0; d < attackedCells.size(); d++)
                {//if attacked cells

                    if (attackedCells[d].c == c && attackedCells[d].r == state2_point.r)
                    {
                        canpush = false;
                    }
                    else
                    {

                    }
                }
                Point tryingtoPush;
                tryingtoPush.r = c;
                tryingtoPush.c = state2_point.r;
                if (!game().isValid(tryingtoPush))
                {
                    canpush = false;
                }
                if (canpush)
                {
                    limitedCells.push_back(Point(state2_point.r, c));//top to bot

                }

                //limitedCells.push_back(Point(c,PointofState2.c));//coordinates for left to right
                //oh shoot if theres a new one u have to pop_back all the limited cells so u dont bait it .
            }


            //last but not least, if all possible checks in that area can be checked is done for, then i have to say to go back to random selection.

        }
        else
        {
            //  cout<<"Resuming state 2 barage"<<endl;
            if (!shotHit)//miss
            {
                state1 = false;
            }
            else if (shotHit && !shipDestroyed)
            {
                state1 = false;
            }
            else if (shotHit && shipDestroyed)
            {
                state1 = true; //i go back to normal
            //    cout<<"Since i sank ur ship im going back to normal me."<<endl;
                /*
                for(int c=0;c<attackedLimitedCells.size();c++)
                {
                    attackedLimitedCells.pop_back();
                }
                 */
            }
        }
    }



}
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
typedef AwfulPlayer GoodPlayer;

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
