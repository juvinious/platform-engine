#ifndef platformer_script_h
#define platformer_script_h

#include "util/pointer.h"

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
        Scriptable();
        virtual ~Scriptable();
        
        /*! Init */
        virtual void init(World *) = 0;
        
        /*! Run through scripted objects and interact with world */
        virtual void act() = 0;
        
        /*! Will render the registered animations that scripted objects may use */
        virtual void render(const Camera &) = 0;
        
        /*! Render an animation created in script */
        virtual void registerAnimation(void *) = 0;
        
        /*! Register a scripted object */
        virtual void registerObject(void *) = 0;
        
        /*! Get Scriptable */
        static Util::ReferenceCount<Scriptable> getScriptable();
        
};

class NoScript : public Scriptable{
    public:
        NoScript();
        virtual ~NoScript();
        
        virtual void init(World *);
        virtual void act();
        virtual void render(const Camera &);
        virtual void registerAnimation(void *);
        virtual void registerObject(void *);
};
}
#endif