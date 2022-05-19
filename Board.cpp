#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;


class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    int board[MAXROWS][MAXCOLS]; 
    const Game& m_game;
    int m_cols;
    int m_rows;

    vector<int> ship_tracker;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    m_rows = g.rows(); 
    m_cols = g.cols();

    for (int i = 0; i < m_rows; i++)
    {
        for (int j = 0; j < m_cols; j++)
        {
            board[i][j] = -1; // board states are initialized 
        }
    }
}

void BoardImpl::clear()
{
    //fill board with '.'
    for (int i = 0; i < m_game.rows(); i++) {
        for (int j = 0; j < m_game.cols(); j++) {
            board[i][j] = '.';
        }
    }

    //clear vectors that store ships
    ship_tracker.clear(); 
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                board[r][c] = -2;
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (board[r][c] == -2)
            {
                board[r][c] += 1; 
            }
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{

    /*
        This function attempts to place the specified ship at the specified coordinate, in the 
        specified direction. The parameter topOrLeft specifies the topmost segment of the ship if 
        dir is VERTICAL, or the leftmost segment if dir is HORIZONTAL. The parameter 
        shipId is the ship ID number. This function returns false if the ship cannot be placed 
        because:
            1. The shipId is invalid
            2. The ship would be partly or fully outside the board.
            3. The ship would overlap an already-placed ship.
            4. The ship would overlap one or more positions that were blocked by a previous 
            call to the block function.
            5. The ship with that ship ID has previously been placed on this Board and not 
            yet been unplaced since its most recent placement
    */

    if (shipId < 0 || shipId >= m_game.nShips())
    {
        return false;
    }

    if (topOrLeft.r >= m_rows || topOrLeft.c >= m_cols || topOrLeft.r < 0 || topOrLeft.c < 0) // check 2, 3, 4
    {
        return false;
    }

    if (dir == HORIZONTAL) // check 2, 3, 4
    {

        if (m_game.shipLength(shipId) + topOrLeft.c > m_cols) // check if there is board space to place the ship 
        {
            return false;
        }
        else // now, check if there is space to place the ship 
        {
            for (int c = topOrLeft.c; c < (m_game.shipLength(shipId) + topOrLeft.c); c++) // since dir = horizontal, we want to loop through every necessary column in a particular row
            {
                if (board[topOrLeft.r][c] != -1) // if position is not empty 
                {
                    return false;
                }
            }
        }

        // all tests passed, now track ship IDs 

        for (int i = 0; i < m_game.nShips(); i++) // check #5 
        {
            if (shipId == i)
            {
                for (int id = 0; id < ship_tracker.size(); id++)
                {
                    if (ship_tracker[id] == shipId)
                    {
                        return false;
                    }
                }
                ship_tracker.push_back(i);
            }
        }

        for (int c = topOrLeft.c; c < (m_game.shipLength(shipId) + topOrLeft.c); c++) // all tests passed, place the ship. 
        { 
            board[topOrLeft.r][c] = shipId;
        }

        return true;
    }

    if (dir == VERTICAL)
    {

        if (m_game.shipLength(shipId) + topOrLeft.r > m_rows) // check if there is board space to place the ship 
        {
            return false;
        }
        else // now, check if there is space to place the ship 
        {
            for (int r = topOrLeft.r; r < (m_game.shipLength(shipId) + topOrLeft.r); r++) // since dir = horizontal, we want to loop through every necessary column in a particular row
            {
                if (board[r][topOrLeft.c] != -1) // if position is not empty 
                {
                    return false;
                }
            }
        }

       
        for (int i = 0; i < m_game.nShips(); i++) // check #5 
        {
            if (shipId == i)
            {
                for (int id = 0; id < ship_tracker.size(); id++)
                {
                    if (ship_tracker[id] == shipId)
                    {
                        return false;
                    }
                }
                ship_tracker.push_back(i);
            }
        }



        for (int r = topOrLeft.r; r < (m_game.shipLength(shipId) + topOrLeft.r); r++) // all tests passed, place the ship. 
        {
            board[r][topOrLeft.c] = shipId;
        }

        return true;
    }

    return false; 
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    /*
    This function attempts to remove the specified ship from the board, so the positions it 
    occupied may be used to place other ships. The parameters are the same as for placeShip. 
    This function returns false if the ship cannot be removed because:

        1. The shipId is invalid
        2. The board does not contain the entire ship at the indicated locations.

    If this function returns false, then the board must remain unchanged (i.e. no part of the 
    ship may be removed upon failure). If this function successfully removes the ship, the 
    board is updated to reflect that, and the function returns true.
    */
    
    if (shipId < 0 || shipId >= m_game.nShips())
    {
        return false;
    }
    
    if (dir == HORIZONTAL)
    {
        if (m_game.shipLength(shipId) + topOrLeft.c > m_cols) // check if there is board space to place the ship 
        {
            return false;
        }

        for (int c = topOrLeft.c; c < (m_game.shipLength(shipId) + topOrLeft.c); c++) // since dir = horizontal, we want to loop through every necessary column in a particular row
        {
            if (board[topOrLeft.r][c] != shipId) // if position is not empty 
            {
                return false;
            }
        }

        // tests passed now remove the ship 



        for (int id = 0; id < ship_tracker.size(); id++)
        {
            if (ship_tracker[id] == shipId)
            {
                ship_tracker.erase(ship_tracker.begin() + id);
            }
        }

        for (int c = topOrLeft.c; c < (m_game.shipLength(shipId) + topOrLeft.c); c++) 
        {
            board[topOrLeft.r][c] = -1; 
        }
    }

    if (dir == VERTICAL)
    {
        if (m_game.shipLength(shipId) + topOrLeft.r > m_rows) // check if there is board space to place the ship 
        {
            return false;
        }

        for (int r = topOrLeft.c; r < (m_game.shipLength(shipId) + topOrLeft.r); r++) // since dir = horizontal, we want to loop through every necessary column in a particular row
        {
            if (board[r][topOrLeft.c] != shipId) // if position is not empty 
            {
                return false;
            }
        }

        // tests passed now remove the ship 



        for (int id = 0; id < ship_tracker.size(); id++)
        {
            if (ship_tracker[id] == shipId)
            {
                ship_tracker.erase(ship_tracker.begin() + id);
            }
        }

        for (int r = topOrLeft.r; r < (m_game.shipLength(shipId) + topOrLeft.r); r++)
        {
            board[r][topOrLeft.c] = -1;
        }
    }

    return false; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
    cout << " ";
    for (int c = 0; c < m_game.cols(); c++)
    {
        cout << c;
    }
    cout << endl;
    for (int i = 0; i < m_game.rows(); i++)
    {
        cout << i; 
        for (int j = 0; j < m_game.cols(); j++)
        {
            if (board[i][j] <= -4) cout << 'X';
            else if (board[i][j] == -3) cout << 'o';
            else if (board[i][j] == -2) cout << '#';
            else if (board[i][j] == -1) cout << '.';
            else cout << m_game.shipSymbol(board[i][j]);
        }
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    /*
    This function is used to submit an attack against the board.The function must return
        false if the attack is invalid(the attack point is outside of the board area, or an attack is
            made on a previously attacked location).The function returns true if the attack is valid,
        regardless of whether or not any ship is damaged.
        If any undamaged segment of a ship is at position p on the board, then the shotHit
        parameter must be set to true, and the segment must henceforth be considered a damaged
        segment.Otherwise the shotHit parameter must be set to false.
        If this specific attack destroyed the last undamaged segment of a ship, then the
        shipDestroyed parameter must be set to true and the shipId parameter must be set to the
        ship ID of the ship that was destroyed; otherwise the shipDestroyed parameter must be
        set to false and shipId must be left unchanged.
        It's up to you whether to set to some value or leave unchanged:
        1. shotHit, shipDestroyed, and shipId if the attack is invalid
        2. shipDestroyed and shipId if the attack missed
        1
    */

    shotHit = false;
    shipDestroyed = false;
    shipId = -1;

    if (p.r < 0 || p.r >= m_rows) // outside of board 
    {
        return false; 
    }
    if (p.c < 0 || p.c >= m_cols)
    {
        return false; 
    }

    if (board[p.r][p.c] <= -3) // attack made on previous attack location
    {
        return false;
    }

    if (board[p.r][p.c] == -1) // hits water 
    {
        board[p.r][p.c] = -3;
        return true;
    }

    if (board[p.r][p.c] >= 0)
    {
        shipId = board[p.r][p.c];
        board[p.r][p.c] = -4; 
        shotHit = true; 

        shipDestroyed = true; 
        for (int i = 0; i < m_rows; i++)
        {
            for (int j = 0; j < m_cols; j++)
            {
                if (board[i][j] == shipId)
                {
                    shipDestroyed = false;
                }
            }
        }

        if (shipDestroyed)
        {
            ship_tracker.insert(ship_tracker.begin() + shipId, -1);
        }
        return true; 
    }

    return false; // if it's == -2
}

bool BoardImpl::allShipsDestroyed() const
{
    int runningSum = 0;

    for (int val : ship_tracker)
    {
        runningSum -= val;
    }

    if (runningSum == m_game.nShips())
    {
        return true; 
    }
  
    return false; // This compiles, but may not be correct
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
