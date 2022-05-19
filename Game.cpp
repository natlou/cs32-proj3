#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
  private:
    int m_rows;
    int m_cols; 

    struct Ship { //struct that stores info for each ship
       Ship(int length, char symbol, string name) : m_length(length), m_symbol(symbol), m_name(name) {}
       int m_length;
       char m_symbol;
       string m_name;
       int ship_id;
    };

    vector<Ship> ships; 
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int rows, int cols) : m_rows(rows), m_cols(cols)
{
    if (m_rows > MAXROWS) {
        m_rows = MAXROWS; //WHAT ELSE CAN I DO HERE IF M_ROW > MAXROWS
    }

    if (m_cols > MAXCOLS) {
        m_cols = MAXCOLS;
    }
}

int GameImpl::rows() const
{
    return m_rows;
}

int GameImpl::cols() const
{
    return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols(); 
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    Ship added_ship(length, symbol, name);
    if ((added_ship.m_length >= MAXROWS) || added_ship.m_length <= 0 ) return false; // check condition

    added_ship.ship_id = ships.size();

    ships.push_back(added_ship);

    return true;
}

int GameImpl::nShips() const
{
    int counter = 0; 

    for (size_t i = 0; i < ships.size(); i++)
    {
        counter += 1; 
    }

    return counter; 
}

int GameImpl::shipLength(int shipId) const
{
    return ships[shipId].m_length;  // This compiles but may not be correct
}

char GameImpl::shipSymbol(int shipId) const
{
    return ships[shipId].m_symbol;  // This compiles but may not be correct
}

string GameImpl::shipName(int shipId) const
{
    return ships[shipId].m_name;  // This compiles but may not be correct
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    bool shot_hit;
    bool ship_destroyed;
    int ship_id;

    Player* result = nullptr;

    if (!(p1->placeShips(b1)))
    {
        return nullptr;
    }
    if (!(p2->placeShips(b2)))
    {
        return nullptr;
    }

    int p1ShipCount = p1->game().nShips();
    int p2ShipCount = p2->game().nShips();
    while (b2.allShipsDestroyed() == false && b1.allShipsDestroyed() == false) //pretty straightforward, wait until a ship is destroyed
    {
        cout << p1->name() << "'s turn. Board for " << p2->name() << ": " << endl;
        b2.display(p1->isHuman());

        Point p1_attacks_p2 = p1->recommendAttack();
        if (b2.attack(p1_attacks_p2, shot_hit, ship_destroyed, ship_id))
        {
            if (shot_hit == false)
            {
                cout << p1->name() << " attacked " << "(" << p1_attacks_p2.r << "," << p1_attacks_p2.c << ")" << " and missed, resulting in: " << endl;
            }
            else
            {
                if (ship_destroyed)
                {
                    cout << p1->name() << " attacked " << "(" << p1_attacks_p2.r << "," << p1_attacks_p2.c << ")" << " and destroyed the " << p2->game().shipName(ship_id) << ", resulting in: " << endl;
                    p2ShipCount -= 1;

                    if (p2ShipCount == 0)
                    {
                        b2.display(p1->isHuman());

                        break;
                    }
                }
                else
                {
                    cout << p1->name() << " attacked " << "(" << p1_attacks_p2.r << "," << p1_attacks_p2.c << ")" << " and hit something, resulting in: " << endl;
                }
            }
            p1->recordAttackResult(p1_attacks_p2, true, shot_hit, ship_destroyed, ship_id);
            p2->recordAttackByOpponent(p1_attacks_p2);
            b2.display(p1->isHuman());
        }

        else
        {
            cout << p1->name() << " wasted a shot at " << "(" << p1_attacks_p2.r << "," << p1_attacks_p2.c << ")" << endl;

        }

        if (p1->isHuman() || p2->isHuman()) {
            if (shouldPause)
                waitForEnter();
        }
        else {
            if (shouldPause)
                waitForEnter();
        }

        cout << p2->name() << "'s turn. Board for " << p1->name() << ": " << endl;

        b1.display(p2->isHuman());

        Point p2_attacks_p1 = p2->recommendAttack();
        if (b1.attack(p2_attacks_p1, shot_hit, ship_destroyed, ship_id))
        { 
            if (shot_hit == false)
            {
                cout << p2->name() << " attacked " << "(" << p2_attacks_p1.r << "," << p2_attacks_p1.c << ")" << " and missed, resulting in: " << endl;
            }
            else
            {
                if (ship_destroyed)
                {
                    cout << p2->name() << " attacked " << "(" << p2_attacks_p1.r << "," << p2_attacks_p1.c << ")" << " and destroyed the " << p1->game().shipName(ship_id) << ", resulting in: " << endl;                    
                    p1ShipCount -= 1;

                    if (p1ShipCount == 0)
                    {
                        b1.display(p2->isHuman());
                        break;
                    }
                }
                else
                {
                    cout << p2->name() << " attacked " << "(" << p2_attacks_p1.r << "," << p2_attacks_p1.c << ")" << " and hit something, resulting in: " << endl;
                }
            }
            p2->recordAttackResult(p2_attacks_p1, true, shot_hit, ship_destroyed, ship_id);
            p1->recordAttackByOpponent(p2_attacks_p1);
            b1.display(p2->isHuman());
        }
        else
        {
            cout << p2->name() << " wasted a shot at " << "(" << p2_attacks_p1.r << "," << p2_attacks_p1.c << ")" << endl;
        }

        
        if (p1->isHuman() || p2->isHuman()) {
            if (shouldPause)
                waitForEnter();
        }
        else {
            if (shouldPause)
                waitForEnter();
        }

    }

    // determine who the winner is, result points to the winner. 
    if (p2ShipCount == 0 && p2->isHuman() && p1ShipCount > 0) 
    {
        b1.display(false);
        result = p1; 

        cout << p1->name() << " wins!" << endl;
    }
    if (p1ShipCount == 0 && p1->isHuman() && p2ShipCount > 0) 
    {
        b2.display(false);
        result = p2; 
        cout << p2->name() << " wins!" << endl;

    }
    if (p1->isHuman() && p2->isHuman() && p2ShipCount == 0 && p1ShipCount > 0)
    {
        b1.display(false);
    }
    if (p1->isHuman() && p2->isHuman() && p1ShipCount == 0 && p2ShipCount > 0)
    {
        b2.display(false);
    }
    if (p1ShipCount == 0 && p2ShipCount > 0) 
    {
        result = p2;
        cout << p2->name() << " wins!" << endl;

    }
    if (p2ShipCount == 0 && p1ShipCount > 0) //if 2 cpus, p1 wins?
    {
        result = p1;
        cout << p1->name() << " wins!" << endl;

    }

    return result;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

