#ifdef HAVE_PYTHON
#include <Python.h>
#endif 

#include "script.h"
#include "object.h"
#include "python.h"
#include "control.h"

#include "resources/camera.h"
#include "game/world.h"
#include "resources/object.h"
#include "resources/collisions.h"
#include "resources/font.h"
#include "resources/value.h"

#include "r-tech1/debug.h"
#include "r-tech1/file-system.h"
#include "r-tech1/funcs.h"
#include "r-tech1/token.h"
#include "r-tech1/exceptions/load_exception.h"

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

static PyObject * getObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    int id;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &id)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Object> object = world->getObject(id);
        
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

static PyObject * getControl(PyObject *, PyObject * args){
    PyObject * worldObject;
    int id = 0;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &id)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Control> control = world->getControl(id);
        
        PyObject * returnable = PyCapsule_New((void *) control.raw(), "control", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * createCamera(PyObject *, PyObject * args){
    PyObject * worldObject;
    int id = 0;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &id)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        /*Util::ReferenceCount<Platformer::Control> control(new Platformer::Script::Control(id));
        world->addControl(control);
        
        PyObject * returnable = PyCapsule_New((void *) control.raw(), "control", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);*/
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getCamera(PyObject *, PyObject * args){
    PyObject * worldObject;
    int id = 0;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &id)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        Util::ReferenceCount<Platformer::Camera> camera = world->getCamera(id);
        
        PyObject * returnable = PyCapsule_New((void *) camera.raw(), "camera", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * followObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    int cameraid = 0;
    int objectid = 0;

    if (PyArg_ParseTuple(args, "Oii", &worldObject, &cameraid, &objectid)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->followObject(cameraid, objectid);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * followNextObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    int cameraid = 0;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &cameraid)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->followNextObject(cameraid);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * lockCameraDown(PyObject *, PyObject * args){
    PyObject * worldObject;
    int cameraid = 0;
    int value = 0;

    if (PyArg_ParseTuple(args, "Oii", &worldObject, &cameraid, &value)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->getCamera(cameraid)->setLockDown((bool)value);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * lockCameraLeft(PyObject *, PyObject * args){
    PyObject * worldObject;
    int cameraid = 0;
    int value = 0;

    if (PyArg_ParseTuple(args, "Oii", &worldObject, &cameraid, &value)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->getCamera(cameraid)->setLockLeft((bool)value);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * lockCameraRight(PyObject *, PyObject * args){
    PyObject * worldObject;
    int cameraid = 0;
    int value = 0;

    if (PyArg_ParseTuple(args, "Oii", &worldObject, &cameraid, &value)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->getCamera(cameraid)->setLockRight((bool)value);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * lockCameraUp(PyObject *, PyObject * args){
    PyObject * worldObject;
    int cameraid = 0;
    int value = 0;

    if (PyArg_ParseTuple(args, "Oii", &worldObject, &cameraid, &value)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->getCamera(cameraid)->setLockUp((bool)value);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * addRuntimeActionFromScript(PyObject *, PyObject * args);
static PyObject * addRuntimeAction(PyObject *, PyObject * args);

static PyObject * throwQuit(PyObject *, PyObject * args){
    PyObject * worldObject;

    if (PyArg_ParseTuple(args, "O", &worldObject)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->requestQuit();
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setPaused(PyObject *, PyObject * args){
    PyObject * worldObject;
    int pause;

    if (PyArg_ParseTuple(args, "Oi", &worldObject, &pause)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        world->setPaused((bool)pause);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getPaused(PyObject *, PyObject * args){
    PyObject * worldObject;

    if (PyArg_ParseTuple(args, "O", &worldObject)){
        Platformer::World * world = reinterpret_cast<Platformer::World*>(PyCapsule_GetPointer(worldObject, "world"));
        return Py_BuildValue("i", world->getPaused());
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * loadPlatformerFont(PyObject *, PyObject * args){
    char * name;
    char * filename;
    char * map;
    int ignoreCase;

    if (PyArg_ParseTuple(args, "sssi", &name, &filename, &map, &ignoreCase)){
        Platformer::Font::Map::add(name, filename, map, (bool)ignoreCase);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getPlatformerFont(PyObject *, PyObject * args){
    PyObject * worldObject;
    char * name;

    if (PyArg_ParseTuple(args, "Os", &worldObject, &name)){
        Util::ReferenceCount<Platformer::Font::Renderer> font = Platformer::Font::Map::get(name);
        PyObject * returnable = PyCapsule_New((void *) font.raw(), "font", NULL);
        if (returnable == NULL){
            PyErr_Print();
        }
        return Py_BuildValue("O", returnable);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * drawText(PyObject *, PyObject * args){
    PyObject * bitmap;
    char * name;
    int x;
    int y;
    int alignment;
    double scale;
    char * text;

    if (PyArg_ParseTuple(args, "sOiiids", &name, &bitmap, &x, &y, &alignment, &scale, &text)){
        const Graphics::Bitmap * work = reinterpret_cast<Graphics::Bitmap *>(PyCapsule_GetPointer(bitmap, "bitmap"));
        Platformer::Font::Map::get(name)->render(*work, x, y, alignment, scale, text);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setValueAsInt(PyObject *, PyObject * args);
static PyObject * getValueAsInt(PyObject *, PyObject * args);
static PyObject * setValueAsDouble(PyObject *, PyObject * args);
static PyObject * getValueAsDouble(PyObject *, PyObject * args);
static PyObject * setValueAsString(PyObject *, PyObject * args);
static PyObject * getValueAsString(PyObject *, PyObject * args);
static PyObject * setValueAsBool(PyObject *, PyObject * args);
static PyObject * getValueAsBool(PyObject *, PyObject * args);

static PyMethodDef Methods[] = {
    {"createObjectFromModule",  createObjectFromModule, METH_VARARGS, "Create a new object from given module and function."},
    {"createObjectFromModuleAt",  createObjectFromModuleAt, METH_VARARGS, "Create a new object at x and y coordinates from given module and function."},
    {"createObject",  createObject, METH_VARARGS, "Create a new object from given module and function."},
    {"createObjectAt",  createObjectAt, METH_VARARGS, "Create a new object at x and y coordinates from given module and function."},
    {"getObject", getObject, METH_VARARGS, "Get an object by id."},
    {"createControl", createControl, METH_VARARGS, "Create a new control."},
    {"getControl", getControl, METH_VARARGS, "Get control by id."},
    {"createCamera", createCamera, METH_VARARGS, "Create a new camera."},
    {"getCamera", getControl, METH_VARARGS, "Get camera by id."},
    {"followObject", followObject, METH_VARARGS, "Follow object by id."},
    {"followNextObject", followNextObject, METH_VARARGS, "Follow next object in objects list."},
    {"lockCameraLeft", lockCameraLeft, METH_VARARGS, "Locks camera from going left."},
    {"lockCameraRight", lockCameraRight, METH_VARARGS, "Locks camera from going right."},
    {"lockCameraUp", lockCameraUp, METH_VARARGS, "Locks camera from going up."},
    {"lockCameraDown", lockCameraDown, METH_VARARGS, "Locks camera from going down."},
    {"addRuntimeActionFromScript", addRuntimeActionFromScript, METH_VARARGS, "Add a runtime action from script that will execute on act or render."},
    {"addRuntimeAction", addRuntimeAction, METH_VARARGS, "Add a runtime action that will execute on act or render."},
    {"throwQuit", throwQuit, METH_VARARGS, "Throw quit exception to shutdown."},
    {"setPaused", setPaused, METH_VARARGS, "Pause game logic."},
    {"getPaused", getPaused, METH_VARARGS, "Get paused state."},
    {"setValueAsInt", setValueAsInt, METH_VARARGS, "Set value as int."},
    {"getValueAsInt", getValueAsInt, METH_VARARGS, "Get value as int."},
    {"setValueAsDouble", setValueAsDouble, METH_VARARGS, "Set value as double."},
    {"getValueAsDouble", getValueAsDouble, METH_VARARGS, "Get value as double."},
    {"setValueAsString", setValueAsString, METH_VARARGS, "Set value as string."},
    {"getValueAsString", getValueAsString, METH_VARARGS, "Get value as string."},
    {"setValueAsBool", setValueAsBool, METH_VARARGS, "Set value as bool."},
    {"getValueAsBool", getValueAsBool, METH_VARARGS, "Get value as bool."},
    {"loadFont", loadPlatformerFont, METH_VARARGS, "Add font in Token form."},
    {"getFont", getPlatformerFont, METH_VARARGS, "Get font by name."},
    {"drawText", drawText, METH_VARARGS, "Draw text to the screen."},
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
    
    void act(bool paused){
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
                PyObject * result = PyObject_CallFunction(function.getObject(), (char *)"OOi", worldObject.getObject(), engine.getObject(), paused);
                if (result == NULL){
                    PyErr_Print();
                }
                Py_DECREF(result);
            }
        }
    }
    
    void render(const Platformer::Camera & camera){
        Platformer::Script::AutoRef worldObject(PyCapsule_New((void *) world, "world", NULL));
        Platformer::Script::AutoRef engine(PyCapsule_New((void *) this, "engine", NULL));
        Platformer::Script::AutoRef bitmap(PyCapsule_New((void *) &camera.getWindow(), "bitmap", NULL));
        if (worldObject.getObject() == NULL || engine.getObject() == NULL || bitmap.getObject() == NULL){
            PyErr_Print();
        } else {
             Platformer::Script::RunMap::iterator render = scripts.find("render");
            if (render != scripts.end()){
                const Platformer::Script::Runnable renderFunction = render->second;
                // run render
                Platformer::Script::AutoRef function = renderFunction.getFunction();
                PyObject * result = PyObject_CallFunction(function.getObject(), (char *)"OOO", worldObject.getObject(), engine.getObject(), bitmap.getObject());
                if (result == NULL){
                    PyErr_Print();
                }
                Py_DECREF(result);
            }
        }
    }
    
    void addImportPath(const std::string & loadpath){
        PyObject * sysPath = PySys_GetObject((char *)"path");
        PyObject * path = PyString_FromString(Storage::instance().find(Filesystem::RelativePath(loadpath + "/")).path().c_str());
        int insertResult = PyList_Insert(sysPath, 0, path);
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
        std::vector<const Token *> animations;
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
                } else if (*tok == "animation"){
                    animations.push_back(tok);
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
        
        Util::ReferenceCount<Platformer::ScriptObject> object(new Platformer::ScriptObject(module, function, animations));
        if (hasPosition){
            object->setX(x);
            object->setY(y);
        }
        world->addObject(object);
    }
    
    void setValue(const std::string & label, const Platformer::Value & value){
        values[label] = value;
    }

    const Platformer::Value getValue(const std::string & label){
        return values[label];
    }

    Platformer::World * world;
    
    // Scripts
    Platformer::Script::RunMap scripts;
    
    // Objects
    typedef std::map<std::string, Util::ReferenceCount<Platformer::Object> > ObjectMap;
    ObjectMap objects;
    
    // Values
    typedef std::map<std::string, Platformer::Value> ValueMap;
    ValueMap values;
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

static PyObject * setValueAsInt(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    int value;
    if (PyArg_ParseTuple(args, "Osi", &scriptPointer,&name, &value)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        python->setValue(name, Platformer::Value(value));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getValueAsInt(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    if (PyArg_ParseTuple(args, "Os", &scriptPointer, &name)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        return Py_BuildValue("i", python->getValue(name).toInt());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setValueAsDouble(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    double value;
    if (PyArg_ParseTuple(args, "Osd", &scriptPointer,&name, &value)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        python->setValue(name, Platformer::Value(value));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getValueAsDouble(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    if (PyArg_ParseTuple(args, "Os", &scriptPointer, &name)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        return Py_BuildValue("d", python->getValue(name).toDouble());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setValueAsString(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    char * value;
    if (PyArg_ParseTuple(args, "Oss", &scriptPointer,&name, &value)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        python->setValue(name, Platformer::Value(value));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getValueAsString(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    if (PyArg_ParseTuple(args, "Os", &scriptPointer, &name)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        return Py_BuildValue("s", python->getValue(name).toString().c_str());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setValueAsBool(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    int value;
    if (PyArg_ParseTuple(args, "Osi", &scriptPointer,&name, &value)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        python->setValue(name, Platformer::Value((bool)value));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getValueAsBool(PyObject *, PyObject * args){
    PyObject * scriptPointer;
    char * name;
    if (PyArg_ParseTuple(args, "Os", &scriptPointer, &name)){
        Python * python = reinterpret_cast<Python*>(PyCapsule_GetPointer(scriptPointer, "engine"));
        return Py_BuildValue("i", python->getValue(name).toBool());
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
        
        void act(bool paused){
        }
        
        void addImportPath(const std::string &){
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
