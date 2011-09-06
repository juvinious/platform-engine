#ifndef platformer_game_h
#define platformer_game_h

#include <vector>

#include "util/input/input-map.h"

class Token;

/*! Platformers game class this is for testing, will be expanded or changed later */

namespace Platformer{
    
class World;
        
class Game{
    public:
	Game(const std::string &);
	virtual ~Game();
	virtual void run();
	
    protected:
	std::vector < Platformer::World *> worlds;
	//! keys
	enum Keys{
	    Up=0,
	    Down,
	    Left,
	    Right,
	    Esc,
        K_1,
        K_2,
        K_3,
        K_4,
	};
	
	InputMap<Keys> input;
};
}
#endif
