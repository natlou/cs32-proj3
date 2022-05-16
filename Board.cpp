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

    vector<int> existingShipIDs;
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
            board[i][j] = 0; // board states are initialized 
        }
    }
}

void BoardImpl::clear()
{
    // This compiles, but may not be correct
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                ; // TODO:  Replace this with code to block cell (r,c)
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            ; // TODO:  Replace this with code to unblock cell (r,c) if blocked
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return false; // This compiles, but may not be correct
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return false; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
    // array dimensions must be constant for now, but we will alter it. 
    char display[MAXCOLS + 3][MAXROWS + 1];

    for (int c = 0; c < m_rows; c++) //for cols numbering, rows = 2
    {
        display[0][c + 1] = '0' + c;
    }
    for (int c = 1; c < m_rows + 1; c++)
    {
        display[1][c] = ' ';
    }
    for (int c = 0; c < m_cols; c++) //numbering for rows. row is 3 size.
    {
        display[c + 2][0] = '0' + c;
    }

    //nrows=2
    //ncols=3
    //cout<<endl; the implementation sampel program doesnt have this line of code.



    cout << "  ";//2 spaces?

    for (int c = 0; c < m_rows; c++)
    {
        for (int r = 0; r < m_cols; r++)
        {
            if (board[r][c] == -1) //for mediocre players
            {
                display[r + 2][c + 1] = '#'; //means blocked.
            }
            if (board[r][c] == 0) 
            {
                display[r + 2][c + 1] = '.';
            }
            if (board[r][c] == 1) 
            {
                display[r + 2][c + 1] = 'o';

            }
            if (board[r][c] > 1) 
            {
                display[r + 2][c + 1] = 'X';
            }
        }
    }



    if(!shotsOnly)
    {
        for (int c = 0; c < m_rows; c++)
        {
            for (int r = 0; r < m_cols; r++)
            {

                for (int d = 0; d < m_game.nShips(); d++)
                {
                    if (display[r][c] == d)
                    {
                        //actually after palcing all the hsips, its just the index of the shipsin the vector, which i have a size function to keep track of.
                        if (existingShipIDs.size() > d)
                        {
                            display[r + 2][c + 1] = m_game.shipSymbol(existingShipIDs[d]);
                        }
                    }
                }
            }
        }
    }



    //if a ship is DAMAGED we must dispaly it as X instead.
    /*
     Remaining lines: The function must print a digit specifying the row number,
     starting at 0, followed by a space, followed by the contents of the current row,
     followed by a newline. You may assume there will be no more than 10 rows. In
     each of the positions of the row, use the following characters to represent the
     playing field:
     a. If the shotOnly parameter is false, use the ship's symbol to display an
     undamaged ship segment; if the shotsOnly parameter is true, show a
     period to display an undamaged ship segment.
     b. Use an X character to display any damaged ship segment.
     c. Use a period to display water where no attack has been made.
     d. Use a lower case letter o character to display water where an attack has
     been made that missed a ship.
     */

     //for hsotsonly it literally means, SHOTS ONLY LOL. so your literally only seeing water or if i hit a ship.

     //now use the shotsonly test to see if I shouldp lace a o or an X, X for displayBoard[r][c]!=0, and true, and a o for displayBoard[r][c]==0
     //basically the code below now doesn't seem necssary at all, ill take it out if it works.
    for (int c = 0; c < m_rows; c++)
    {
        for (int r = 0; r < m_cols; r++)
        {

            if (board[r][c] > 1000) //a ship!
            { //so a ship. I should verify that my r and c is not going to be hitting any of the 0-9 on the cout board.
                display[r + 2][c + 1] = 'X';
            }
            if (board[r][c] == 1000) //a hit water.
            {
                display[r + 2][c + 1] = 'o';
            }
            if (board[r][c] == -2) //for mediocre players
            {
                display[r + 2][c + 1] = '#'; //means blocked.
            }
        }
        //endl;
    }

    //how about just straight up hardcode first row, ignore the first row here, and then print rest

    if (!shotsOnly) //everytjing allowed
    {
        for (int c = 0; c < m_rows + 1; c++) //1 space with number of rows
        {
            for (int r = 0; r < m_cols + 2; r++) //should be 2 spaces due to number of cols
            {
                if (c == 0 && r < 2) //r>9
                {
                    //cout<<r<<endl;
                    continue;
                }


                cout << display[r][c];
            }
            cout << endl;
        }
    }
    if (shotsOnly) //everytjing not allowed
    {
        for (int c = 0; c < m_rows + 1; c++) //1 space with number of rows
        {
            for (int r = 0; r < m_cols + 2; r++) //should be 2 spaces due to number of cols
            {
                if (c == 0 && r < 2) //r>9
                {
                    //cout<<r<<endl;
                    continue;
                }
                if (r > 1 && c > 0)
                {
                    if (display[r][c] == 'X' || display[r][c] == 'o' || display[r][c] == '.')
                    {
                        cout << display[r][c];
                    }
                    else
                    {
                        cout << '.';//its just a normal water if we don't know what it is.
                    }
                }
                else
                {
                    cout << display[r][c]; //prints out the number of row and col
                }

            }
            cout << endl;
        }
    }

}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return false; // This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const
{
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
