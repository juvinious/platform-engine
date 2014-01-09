#ifndef platformer_world_h
#define platformer_world_h

#include <string>
#include <vector>
#include <map>

#include "util/pointer.h"
#include "util/graphics/bitmap.h"

namespace Path{
class AbsolutePath;
}
class Token;

/*! Platformers world class */

namespace Platformer{
    
class Animation;
class Background;
class Camera;
class Scriptable;
class Object;

namespace Collisions{
class Map;
}
    
class World{
public:
    World(const Token *);
    virtual ~World();
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual void setCamera(int id, double x, double y);
    virtual void moveCamera(int id, double x, double y);
    
    //! FIXME HANDLE Cameras correctly later
    virtual Util::ReferenceCount<Camera> getCamera(int id);
    
    virtual void addObject(Util::ReferenceCount<Object>);
    
    void invokeScript(const std::string &, const std::string &);
    
    inline virtual int getResolutionX() const {
        return this->resolutionX;
    }
    
    inline virtual int getResolutionY() const {
        return this->resolutionY;
    }
    
protected:
    virtual void load(const Path::AbsolutePath &);
    virtual void load(const Token *);
    //! Name
    std::string name;
    //! Resolution of map (upscaled/downscaled according to paintowns screensize)
    int resolutionX;
    int resolutionY;
    //! Dimensions of the entire map
    int dimensionsX;
    int dimensionsY;
    
    //! mechanics
    double gravityX;
    double gravityY;
    double acceleration;
    
    //! Fill color
    Graphics::Color fillColor;
    
    // Player info
    
    //! Cameras
    std::map< int, Util::ReferenceCount<Camera> > cameras;
    
    //! Animation map
    std::map< std::string, Util::ReferenceCount<Animation> > animations;
    
    //! Backgrounds
    std::vector< Util::ReferenceCount<Background> > backgrounds;
    
    //! Foregrounds
    std::vector< Util::ReferenceCount<Background> > foregrounds;
    
    //! Collision map
    Util::ReferenceCount<Collisions::Map> collisionMap;
    
    //! Objects
    std::vector< Util::ReferenceCount<Object> > objects;
    
    //! Script engine
    Util::ReferenceCount<Scriptable> scriptEngine;
};
}
#endif
