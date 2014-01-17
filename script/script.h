#ifndef platformer_script_h
#define platformer_script_h

#include "util/pointer.h"

#include <string>

class Token;

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
    virtual void act(bool paused) = 0;
    
    /*! Will render the registered animations that scripted objects may use */
    virtual void render(const Camera &) = 0;
    
    /*! Add an import path to script engine */
    virtual void addImportPath(const std::string &) = 0;
    
    /*! Load a user script and execute the function */
    virtual void runScript(const std::string &, const std::string &) = 0;
    
    /*! Load script from token */
    virtual void importObject(const Token *) = 0;
    
    /*! Get Scriptable */
    static Util::ReferenceCount<Scriptable> getInstance(World *);
    
};

}
#endif
