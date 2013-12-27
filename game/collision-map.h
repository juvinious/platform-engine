#ifndef platformer_collision_map_h
#define platformer_collision_map_h

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

class CollisionBody{
public:
    CollisionBody(){}
    virtual ~CollisionBody(){}
    
    virtual void collided(const CollisionInfo &) = 0;
    
    inline const Area & getMain() const {
        return this->main;
    }
    inline const Area & getTop() const {
        return this->top;
    }
    inline const Area & getBottom() const {
        return this->bottom;
    }
    inline const Area & getLeft() const {
        return this->left;
    }
    inline const Area & getRight() const {
        return this->right;
    }
    
protected:
    Area main;
    Area top;
    Area bottom;
    Area left;
    Area right;
};

class CollisionMap{
public:
    CollisionMap(const Token *);
    virtual ~CollisionMap();
    
    bool collides(Util::ReferenceCount<CollisionBody>);
    
    void act();
    
    void render(const Camera &);
    
protected:
    std::vector<Area> regions;
};

}
#endif