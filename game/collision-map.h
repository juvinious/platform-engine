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

struct CollisionInfo{
    enum CollisionType{
        None,
        Top,
        Bottom,
        Left,
        Right,
    };
    CollisionType type;
    Area area;
};

class CollisionMap{
public:
    CollisionMap(const Token *);
    virtual ~CollisionMap();
    
    CollisionInfo collides(Util::ReferenceCount<Object>);
    
    void render(const Camera &);
    
protected:
    std::vector<Area> regions;
};

}
#endif