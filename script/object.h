#ifndef platformer_script_object_h
#define platformer_script_object_h

#ifdef HAVE_PYTHON

#include "util/pointer.h"
#include "platformer/object/object.h"
#include "platformer/game/animation.h"

#include <string>
#include <map>

class PyMethodDef;

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
    Runnable();
    Runnable(const std::string &, const std::string &);
    Runnable(const Runnable &);
    ~Runnable();
    
    const Runnable & operator=(const Runnable &);
    
    const std::string & getModule() const {
        return this->module;
    }
    const std::string & getFunction() const {
        return this->function;
    }
    
private:
    std::string module;
    std::string function;
};

typedef std::map<std::string, Runnable> RunMap;

class ScriptObject : public Object{
public:
    ScriptObject(const std::string &, const std::string &);
    virtual ~ScriptObject();
    
    void act(const Util::ReferenceCount<Platformer::CollisionMap>, std::vector< Util::ReferenceCount<Object> > &);
    void draw(const Platformer::Camera &);
    
    void add(const std::string &, const Runnable &);
    
    
    static PyMethodDef * Methods;
private:

    std::map<std::string, Util::ReferenceCount<Animation> > animations;
    RunMap scripts;
};

}
#endif
#endif
