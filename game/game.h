#ifndef platformer_game_h
#define platformer_game_h

#include <map>
#include <string>
#include <vector>

#include "util/input/input-map.h"
#include "util/pointer.h"

class Token;

namespace Gui{   
class CutScene;
}

/*! Platformers game class this is for testing, will be expanded or changed later */

namespace Platformer{
    
class World;
        
class Game{
    public:
	Game(const std::string &);
	virtual ~Game();
	virtual void run();
	
    protected:
        std::map < std::string, Util::ReferenceCount<Gui::CutScene> > cutscenes;
	std::vector < Util::ReferenceCount<Platformer::World> > worlds;
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
