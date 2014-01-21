#ifndef platformer_collisions_h
#define platformer_collisions_h

#include <string>
#include <vector>
#include <map>

#include "util/token.h"
#include "util/pointer.h"

namespace Platformer{
    
class Camera;
class Object;

namespace Collisions {

class Area{
public:
    Area();
    Area(double x, double y, int width, int height);
    Area(const Token *, const std::string & name = "area");
    Area(const Area &);
    ~Area();
    double x;
    double y;
    int width;
    int height;
    
    const Area & operator=(const Area &);
    
    inline double getX2() const {
        return this->x + this->width;
    }
    
    inline double getY2() const {
        return this->y + this->height;
    }
};

struct Info{
    bool top;
    bool bottom;
    bool right;
    bool left;
    Area area;
};

class Body{
public:
    Body();
    virtual ~Body();
    
    virtual void response(const Collisions::Info &) const = 0;
    
    virtual void noCollision() const;
    
    bool collides(const Area &) const;
    
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

class Map{
public:
    Map(const Token *);
    virtual ~Map();
    
    void collides(const Collisions::Body &) const;
    
    void act();
    
    void render(const Camera &);
    
protected:
    std::vector<Area> regions;
};

} // Collisions
}
#endif
