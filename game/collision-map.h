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
    inline int getX2() const {
        return this->x + this->width;
    }
    inline int getY2() const {
        return this->y + this->height;
    }
};

struct CollisionInfo{
    bool top;
    bool bottom;
    bool right;
    bool left;
    Area area;
};

class CollisionBody{
public:
    CollisionBody();
    virtual ~CollisionBody();
    
    virtual void response(const CollisionInfo &) const = 0;
    
    virtual void noCollision() const {};
    
    inline const Area & getArea() const {
        return this->area;
    }
    
    inline double getVelocityX() const {
        return this->velocityX;
    }
    
    inline double getVelocityY() const {
        return this->velocityY;
    }
    
protected:
    Area area;
    double velocityX;
    double velocityY;
};

class CollisionMap{
public:
    CollisionMap(const Token *);
    virtual ~CollisionMap();
    
    void collides(const CollisionBody &) const;
    
    void act();
    
    void render(const Camera &);
    
protected:
    std::vector<Area> regions;
};

}
#endif
