#ifdef HAVE_PYTHON
#include <Python.h>
#endif 

#include "script.h"
#include "object.h"
#include "python.h"
#include "control.h"

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
        
        PyObject * returnable = PyCapsule_New((void *) object.raw(), "object", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
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
        
        PyObject * returnable = PyCapsule_New((void *) object.raw(), "object", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
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
        
        PyObject * returnable = PyCapsule_New((void *) object.raw(), "object", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
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
        
        PyObject * returnable = PyCapsule_New((void *) object.raw(), "object", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject * createControl(PyObject *, PyObject * args){
    PyObject * worldObject;
    int id = 0;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &id)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Control> control(new Platformer::Script::Control(id));
        world->addControl(control);
        
        PyObject * returnable = PyCapsule_New((void *) control.raw(), "control", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * addRuntimeActionFromScript(PyObject *, PyObject * args);
static PyObject * addRuntimeAction(PyObject *, PyObject * args);

static PyMethodDef Methods[] = {
    {"createObjectFromModule",  createObjectFromModule, METH_VARARGS, "Create a new object from given module and function."},
    {"createObjectFromModuleAt",  createObjectFromModuleAt, METH_VARARGS, "Create a new object at x and y coordinates from given module and function."},
    {"createObject",  createObject, METH_VARARGS, "Create a new object from given module and function."},
    {"createObjectAt",  createObjectAt, METH_VARARGS, "Create a new object at x and y coordinates from given module and function."},
    {"createControl", createControl, METH_VARARGS, "Create a new control."},
    {"addRuntimeActionFromScript", addRuntimeActionFromScript, METH_VARARGS, "Add a runtime action from script that will execute on act or render."},
    {"addRuntimeAction", addRuntimeAction, METH_VARARGS, "Add a runtime action that will execute on act or render."},
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
        Py_InitModule("platformer_control", Platformer::Script::Control::Methods);
    }
    
    virtual ~Python(){
        Py_Finalize();
    }
    
    void act(){
        Platformer::Script::AutoRef worldObject(PyCapsule_New((void *) world, "world", NULL));
        Platformer::Script::AutoRef engine(PyCapsule_New((void *) this, "engine", NULL));
        if (worldObject.getObject() == NULL || engine.getObject() == NULL){
            PyErr_Print();
        } else {
             Platformer::Script::RunMap::iterator act = scripts.find("act");
            if (act != scripts.end()){
                const Platformer::Script::Runnable actFunction = act->second;
                // run act
                Platformer::Script::AutoRef function = actFunction.getFunction();
                PyObject * result = PyObject_CallFunction(function.getObject(), (char *)"OO", worldObject.getObject(), engine.getObject());
                if (result == NULL){
                    PyErr_Print();
                }
                Py_DECREF(result);
            }
        }
    }
    
    void render(const Platformer::Camera & camera){
    }
    
    void runScript(const std::string & module, const std::string & func){
        Platformer::Script::Runnable runnable(module, func);
        Platformer::Script::AutoRef worldObject(PyCapsule_New((void *) world, "world", NULL));
        if (worldObject.getObject() == NULL){
            PyErr_Print();
        }
        
        Platformer::Script::AutoRef engine(PyCapsule_New((void *) this, "engine", NULL));
        if (engine.getObject() == NULL){
            PyErr_Print();
        }
        
        // Execute
        Platformer::Script::AutoRef function = runnable.getFunction();
        PyObject * result = PyObject_CallFunction(function.getObject(), (char*) "OO", worldObject.getObject(), engine.getObject());
        if (result == NULL){
            PyErr_Print();
        }
        Py_DECREF(result);
    }
    
    /*! Add a runtime action that runs on each act */
    void addRuntimeAction(const std::string & what, const Platformer::Script::Runnable & runnable){
        scripts.insert(std::pair<std::string, Platformer::Script::Runnable>(what, runnable));
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
    
    // Scripts
    Platformer::Script::RunMap scripts;
};


static PyObject * addRuntimeActionFromScript(PyObject *, PyObject * args){
    PyObject * enginePointer;
    char * action;
    char * module;
    char * function;
    if (PyArg_ParseTuple(args, "Osss", &enginePointer, &action, &module, &function)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(enginePointer, "engine"));
        python->addRuntimeAction(action, Platformer::Script::Runnable(module, function));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * addRuntimeAction(PyObject *, PyObject * args){
    PyObject * enginePointer;
    char * action;
    PyObject * func;
    if (PyArg_ParseTuple(args, "OsO", &enginePointer, &action, &func)){
        Platformer::Script::AutoRef function(func);
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(enginePointer, "engine"));
        python->addRuntimeAction(action, Platformer::Script::Runnable(function));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

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
        
        void runScript(const std::string &, const std::string &){
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
