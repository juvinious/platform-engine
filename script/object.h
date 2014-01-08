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

class AutoRef{
public:
    AutoRef(PyObject *);
    AutoRef(const AutoRef &);
    ~AutoRef();
    
    const AutoRef & operator=(const AutoRef &);
    
    PyObject * getObject() const;
    
private:
    PyObject * object;
};

class Runnable{
public:
    Runnable(const std::string &, const std::string &);
    Runnable(const Runnable &);
    ~Runnable();
    
    const Runnable & operator=(const Runnable &);
    
    const AutoRef getModule() const;
    PyObject * getFunction() const;
    
    const std::string & getModuleName() const {
        return this->moduleName;
    }
    const std::string & getFunctionName() const {
        return this->functionName;
    }
    
private:
    std::string moduleName;
    std::string functionName;
    typedef std::map<std::string, AutoRef> RefMap;
    static RefMap modules;
    static RefMap functions;
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
