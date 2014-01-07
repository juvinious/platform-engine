#ifndef platformer_script_object_h
#define platformer_script_object_h

#ifdef HAVE_PYTHON

#include "util/pointer.h"
#include "platformer/object/object.h"
#include "platformer/game/animation.h"

#include <string>
#include <map>

namespace Graphics{
class Bitmap;
}

namespace Platformer{
    
class Camera;
class Object;
class World;
class Animation;

class Runnable{
public:
    Runnable(const std::string, const std::string);
    Runnable(const Runnable &);
    ~Runnable();
    
    const Runnable & operator=(const Runnable &);
    
    const std::string & getModule() const {
        return this->module;
    }
    const std::string & getFunction() const {
        return this->module;
    }
    
private:
    std::string module;
    std::string function;
};

class ScriptObject : public Object{
public:
    ScriptObject();
    virtual ~ScriptObject();
    
    void act(const Util::ReferenceCount<Platformer::CollisionMap> collisionMap);
    void draw(const Platformer::Camera & camera);
private:

    std::map<std::string, Util::ReferenceCount<Animation> > animations;
    std::map<std::string, Runnable> scripts;
};

}
#endif
#endif
