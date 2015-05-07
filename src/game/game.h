#ifndef platformer_game_h
#define platformer_game_h

#include <map>
#include <string>
#include <vector>

#include "util/pointer.h"

class Token;

namespace Gui{   
class CutScene;
}

/*! Platformers game class this is for testing, will be expanded or changed later */

namespace Platformer{
    
class Object;
class World;
        
class Game{
public:
    Game(const std::string &);
    virtual ~Game();
    virtual void run();
    
protected:
    //std::map < std::string, Util::ReferenceCount<Gui::CutScene> > cutscenes;
    Util::ReferenceCount<Platformer::World> world;
};
}
#endif
