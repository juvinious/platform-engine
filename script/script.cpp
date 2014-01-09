#ifdef HAVE_PYTHON
#include <Python.h>
#endif 

#include "script.h"
#include "object.h"
#include "python.h"

#include "platformer/resources/camera.h"
#include "platformer/game/world.h"
#include "platformer/resources/object.h"
#include "platformer/resources/collisions.h"

#include "util/file-system.h"
#include "util/funcs.h"

#ifdef HAVE_PYTHON

static PyObject * createObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    char * module;
    char * initFunction;

    if (PyArg_ParseTuple(args, "Oss", &worldObject, &module, &initFunction)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Object> object(new Platformer::ScriptObject(module, initFunction));
        world->addObject(object);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Methods[] = {
    {"createObject",  createObject, METH_VARARGS, "Create a new object."},
    {NULL, NULL, 0, NULL}
};

// Python script support
class Python : public Platformer::Scriptable{
public:
    Python(Platformer::World * world):
    world(world){
        Py_Initialize();
        Py_InitModule("platformer", Methods);
        Py_InitModule("platformer_object", Platformer::ScriptObject::Methods);
    }
    
    virtual ~Python(){
        Py_Finalize();
    }
    
    void act(){
    }
    
    void render(const Platformer::Camera & camera){
    }
    
    void loadScript(const std::string & module, const std::string & func){
        Platformer::Script::Runnable runnable(module, func);
        PyObject * worldObject = PyCapsule_New((void *) world, "world", NULL);
        if (worldObject == NULL){
            PyErr_Print();
        }
        
        // Execute
        Platformer::Script::AutoRef function = runnable.getFunction();
        PyObject * result = PyObject_CallFunction(function.getObject(), (char*) "(O)", worldObject);
        if (result == NULL){
            PyErr_Print();
        }
        Py_DECREF(worldObject);
        Py_DECREF(result);
    }

    Platformer::World * world;
};

#endif

Util::ReferenceCount<Platformer::Scriptable> Platformer::Scriptable::getInstance(World * world){
#ifdef HAVE_PYTHON
    return Util::ReferenceCount<Platformer::Scriptable>(new Python(world));
#endif
    // No script support
    class NoScript : public Platformer::Scriptable{
    public:
        NoScript(){
        }
        virtual ~NoScript(){
        }
        
        void act(){
        }
        
        void loadScript(const std::string &, const std::string &){
        }
        
        void render(const Camera &){
        }
        void registerAnimation(void *){
        }
    };
    return Util::ReferenceCount<Platformer::Scriptable>(new NoScript());
}
