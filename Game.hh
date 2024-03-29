#ifndef Game_hh
#define Game_hh

#include "Utils.hh"
#include "PosDir.hh"
#include "Board.hh"
#include "Action.hh"
#include "Player.hh"

using namespace std;


/**
 * Game class.
 */

class Game {

public:

	static int run(vector<string> names, istream& is, ostream& os);

};


#endif


