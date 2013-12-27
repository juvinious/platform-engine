#ifndef platformer_collision_map_h
#define platformer_collinion_map_h

#include <string>
#include <vector>
#include <map>

#include "util/token.h"
#include "util/pointer.h"

namespace Platformer{
    
class Camera;
class Object;

struct Area{
    int x;
    int y;
    int width;
    int height;
};

class CollisionMap{
public:
    CollisionMap(const Token *);
    virtual ~CollisionMap();
    
    bool collides(Util::ReferenceCount<Object>);
    
    void render(const Camera &);
    
protected:
    std::vector<Area> regions;
};

}
#endif