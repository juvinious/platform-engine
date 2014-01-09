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

#include "util/debug.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/token.h"
#include "util/exceptions/load_exception.h"

#ifdef HAVE_PYTHON

static PyObject * createObjectFromModule(PyObject *, PyObject * args){
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

static PyObject * createObjectFromModuleAt(PyObject *, PyObject * args){
    PyObject * worldObject;
    char * module;
    char * initFunction;
    double x = 0;
    double y = 0;

    if (PyArg_ParseTuple(args, "Ossdd", &worldObject, &module, &initFunction, &x, &y)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Object> object(new Platformer::ScriptObject(module, initFunction));
        object->setX(x);
        object->setY(y);
        world->addObject(object);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * createObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    PyObject * func;

    if (PyArg_ParseTuple(args, "OO", &worldObject, &func)){
        Platformer::Script::AutoRef function(func, true);
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Object> object(new Platformer::ScriptObject(Platformer::Script::Runnable(function)));
        world->addObject(object);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * createObjectAt(PyObject *, PyObject * args){
    PyObject * worldObject;
    PyObject * func;
    double x = 0;
    double y = 0;

    if (PyArg_ParseTuple(args, "OOdd", &worldObject, &func, &x, &y)){
        Platformer::Script::AutoRef function(func, true);
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Object> object(new Platformer::ScriptObject(Platformer::Script::Runnable(function)));
        object->setX(x);
        object->setY(y);
        world->addObject(object);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Methods[] = {
    {"createObjectFromModule",  createObjectFromModule, METH_VARARGS, "Create a new object from given module and function."},
    {"createObjectFromModuleAt",  createObjectFromModuleAt, METH_VARARGS, "Create a new object at x and y coordinates from given module and function."},
    {"createObject",  createObject, METH_VARARGS, "Create a new object from given module and function."},
    {"createObjectAt",  createObjectAt, METH_VARARGS, "Create a new object at x and y coordinates from given module and function."},
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
        PyObject * result = PyObject_CallFunction(function.getObject(), (char*) "O", worldObject);
        if (result == NULL){
            PyErr_Print();
        }
        Py_DECREF(worldObject);
        Py_DECREF(result);
    }
    
    void importObject(const Token * token){
        if ( *token != "object-script" ){
            throw LoadException(__FILE__, __LINE__, "Not an object script.");
        }
        std::string module, function;
        bool hasPosition = false;
        int x=0, y=0;
        TokenView view = token->view();
        while (view.hasMore()){
            try{
                const Token * tok;
                view >> tok;
                if (*tok == "module"){
                    tok->view() >> module;
                } else if (*tok == "function"){
                    tok->view() >> function;
                } else if (*tok == "position"){
                    tok->view() >> x >> y;
                    hasPosition = true;
                } else {
                    Global::debug( 3 ) << "Unhandled object script attribute: "<< std::endl;
                    if (Global::getDebug() >= 3){
                        token->print(" ");
                    }
                }
            } catch ( const TokenException & ex ) {
                throw LoadException(__FILE__, __LINE__, ex, "Object script parse error");
            } catch ( const LoadException & ex ) {
                throw ex;
            }
        }
        
        Util::ReferenceCount<Platformer::Object> object(new Platformer::ScriptObject(module, function));
        if (hasPosition){
            object->setX(x);
            object->setY(y);
        }
        world->addObject(object);
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
        void importObject(const Token *){
        }
    };
    return Util::ReferenceCount<Platformer::Scriptable>(new NoScript());
}
