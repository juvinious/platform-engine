#ifndef platformer_script_h
#define platformer_script_h

#include "util/pointer.h"

#include <string>

namespace Graphics{
class Bitmap;
}

namespace Platformer{
    
class Camera;
class Object;
class World;

/*! Scriptable which is managed by world is for handling of scripted objects */
class Scriptable{
public:
    Scriptable(){}
    virtual ~Scriptable(){}
    
    /*! Run through scripted objects and interact with world */
    virtual void act() = 0;
    
    /*! Will render the registered animations that scripted objects may use */
    virtual void render(const Camera &) = 0;
    
    /*! Render an animation created in script */
    virtual void registerAnimation(void *) = 0;
    
    /*! Register a scripted object */
    virtual void registerObject(void *) = 0;
    
    /*! Load a user script */
    virtual void loadScript(const std::string &) = 0;
    
    /*! Get Scriptable */
    static Util::ReferenceCount<Scriptable> getInstance(World *);
    
};

}
#endif