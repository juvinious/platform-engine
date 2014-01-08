#ifdef HAVE_PYTHON
#include <Python.h>
#include "object.h"
#include "platformer/game/camera.h"
#include "platformer/game/collision-map.h"

#include "util/graphics/bitmap.h"
#include "util/file-system.h"
#include "util/funcs.h"

using namespace Platformer;

AutoRef::AutoRef(PyObject * object):
object(object){
}

AutoRef::AutoRef(const AutoRef & copy):
object(copy.object){
    Py_XINCREF(object);
}

AutoRef::~AutoRef(){
    Py_XDECREF(object);
}

const AutoRef & AutoRef::operator=(const AutoRef & copy){
    object = copy.object;
    Py_XINCREF(object);
    
    return *this;
}

PyObject * AutoRef::getObject() const {
    return object;
}

Runnable::RefMap Runnable::modules;

Runnable::Runnable(const std::string & moduleName, const std::string & functionName):
moduleName(moduleName),
functionName(functionName){
    RefMap::iterator module = modules.find(moduleName);
    if (module == modules.end()){
        PyObject * sysPath = PySys_GetObject((char *)"path");
        // FIXME Do not use a fixed location but for now make it data/platformer
        PyObject * path = PyString_FromString(Storage::instance().find(Filesystem::RelativePath("platformer/")).path().c_str());
        int insertResult = PyList_Insert(sysPath, 0, path);
        
        // Import the module
        PyObject * importedModule = PyImport_ImportModule(moduleName.c_str());
        if (PyErr_Occurred()){
            PyErr_Print();
        }
        
        modules.insert(std::pair<std::string, AutoRef>(moduleName, importedModule));
    }
}

Runnable::Runnable(const Runnable & copy):
moduleName(copy.moduleName),
functionName(copy.functionName){
}

Runnable::~Runnable(){
}

const Runnable & Runnable::operator=(const Runnable & copy){
    moduleName = copy.moduleName;
    functionName = copy.functionName;
    
    return *this;
}

const AutoRef Runnable::getModule() const {
    return modules.find(moduleName)->second;
}

PyObject * Runnable::getFunction() const {
    PyObject * retrievedFunction = PyObject_GetAttrString(getModule().getObject(), functionName.c_str());
    if (retrievedFunction == NULL){
        PyErr_Print();
    }
    return retrievedFunction;
}

static PyObject * getID(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        return Py_BuildValue("i", obj->getID());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getLabel(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("s", obj->getLabel().c_str());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setLabel(PyObject *, PyObject * args){
    PyObject * charPointer;
    char * label;
    if (PyArg_ParseTuple(args, "Os", &charPointer, &label)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setLabel(label);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getX(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("d", obj->getX());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setX(PyObject *, PyObject * args){
    PyObject * charPointer;
    double x = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &x)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setX(x);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getY(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("d", obj->getY());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setY(PyObject *, PyObject * args){
    PyObject * charPointer;
    double y = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &y)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setY(y);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getWidth(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("i", obj->getWidth());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setWidth(PyObject *, PyObject * args){
    PyObject * charPointer;
    int width = 0;
    if (PyArg_ParseTuple(args, "Oi", &charPointer, &width)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setWidth(width);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getHeight(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("i", obj->getHeight());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setHeight(PyObject *, PyObject * args){
    PyObject * charPointer;
    int height = 0;
    if (PyArg_ParseTuple(args, "Oi", &charPointer, &height)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setHeight(height);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getVelocityX(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("d", obj->getVelocityX());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setVelocityX(PyObject *, PyObject * args){
    PyObject * charPointer;
    double velocity = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &velocity)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setVelocityX(velocity);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getVelocityY(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");    
        return Py_BuildValue("d", obj->getVelocityY());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setVelocityY(PyObject *, PyObject * args){
    PyObject * charPointer;
    double velocity = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &velocity)){
        Object * obj = (Object*) PyCapsule_GetPointer(charPointer, "object");
        obj->setVelocityY(velocity);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * addScript(PyObject *, PyObject * args){
    PyObject * charPointer;
    char * action;
    char * module;
    char * function;
    if (PyArg_ParseTuple(args, "Osss", &charPointer, &action, &module, &function)){
        ScriptObject * obj = (ScriptObject*) PyCapsule_GetPointer(charPointer, "object");
        obj->add(action, Runnable(module, function));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef ObjectMethods[] = {
    {"getID", getID, METH_VARARGS, "Get ID."},
    {"getLabel", getLabel, METH_VARARGS, "Get label."},
    {"setLabel", setLabel, METH_VARARGS, "Set label."},
    {"getX", getX, METH_VARARGS, "Get x."},
    {"setX", setX, METH_VARARGS, "Set x."},
    {"getY", getY, METH_VARARGS, "Get y."},
    {"setY", setY, METH_VARARGS, "Set y."},
    {"getWidth", getWidth, METH_VARARGS, "Get width."},
    {"setWidth", setWidth, METH_VARARGS, "Set width."},
    {"getHeight", getHeight, METH_VARARGS, "Get height."},
    {"setHeight", setHeight, METH_VARARGS, "Set height."},
    {"getVelocityX", getVelocityX, METH_VARARGS, "Get x velocity."},
    {"setVelocityX", setVelocityX, METH_VARARGS, "Set x velocity."},
    {"getVelocityY", getVelocityY, METH_VARARGS, "Get y velocity."},
    {"setVelocityY", setVelocityY, METH_VARARGS, "Set y velocity."},
    {"addScript", addScript, METH_VARARGS, "Add a script."},
    {NULL, NULL, 0, NULL}
};

PyMethodDef * ScriptObject::Methods = ObjectMethods;

// Decrement after done
static PyObject * getModule(const std::string & module){
    PyObject * sysPath = PySys_GetObject((char *)"path");
    // FIXME Do not use a fixed location but for now make it data/platformer
    PyObject * path = PyString_FromString(Storage::instance().find(Filesystem::RelativePath("platformer/")).path().c_str());
    int insertResult = PyList_Insert(sysPath, 0, path);
    
    // Import the module
    PyObject * loadModule = PyImport_ImportModule(module.c_str());
    if (PyErr_Occurred()){
        // TODO Throw an exception for now print
        PyErr_Print();
    }
    
    return loadModule;
}

ScriptObject::ScriptObject(const std::string & initModule, const std::string & initFunction){
    // Run init function
    Runnable init(initModule, initFunction);
    PyObject * self = PyCapsule_New((void *) this, "object", NULL);
    if (self == NULL){
        PyErr_Print();
    }
    
    // Execute init
    PyObject * function = init.getFunction();
    PyObject * result = PyObject_CallFunction(function, (char*) "(O)", self);
    if (result == NULL){
        PyErr_Print();
    }
    Py_DECREF(self);
    Py_DECREF(function);
    Py_XDECREF(result);
}

ScriptObject::~ScriptObject(){
}

void ScriptObject::act(const Util::ReferenceCount<Platformer::CollisionMap> collisionMap, std::vector< Util::ReferenceCount<Object> > & objects){
    // Act
    RunMap::iterator act = scripts.find("act-object");
    if (act != scripts.end()){
        const Runnable actFunction = act->second;
        PyObject * self = PyCapsule_New((void *) this, "object", NULL);
        if (self == NULL){
            PyErr_Print();
        } else { 
            for (std::vector< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); i++){
                Py_INCREF(self);
                PyObject * function = actFunction.getFunction();
                Py_INCREF(function);
                if (function == NULL){
                    PyErr_Print();
                    continue;
                }        
                PyObject * object = PyCapsule_New((void *) (*i).raw(), "object", NULL);
                if (object == NULL){
                    PyErr_Print();
                    Py_DECREF(function);
                    Py_DECREF(self);
                    continue;
                }
                // run act-object
                PyObject * result = PyObject_CallFunction(function, (char *)"(OO)", self, object);
                if (result == NULL){
                    PyErr_Print();
                }
                Py_DECREF(self);
                Py_DECREF(object);
                Py_DECREF(function);
                Py_XDECREF(result);
            }
        }
        Py_XDECREF(self); 
    }
    
    class Collider : public CollisionBody{
    public:
        Collider(ScriptObject & object):
        object(object){
            area.x = object.getX();
            area.y = object.getY();
            area.width = object.getWidth();
            area.height = object.getHeight();
            velocityX = object.getVelocityX();
            velocityY = object.getVelocityY();
        }
        ~Collider(){}
        ScriptObject & object;
        
        void response(const CollisionInfo & info) const {
            if (info.top){
                object.setVelocityY(0);
            }
            if (info.bottom){
                object.setVelocityY(0);
            }
            if (info.left){
                object.setVelocityX(0);
            }
            if (info.right){
                object.setVelocityX(0);
            }
        }
    };
    
    Collider collider(*this);
    collisionMap->collides(collider);
    
    x += velocityX;
    y += velocityY;    
}

void ScriptObject::draw(const Platformer::Camera & camera){
    if (x >= (camera.getX() - width) && 
            x <= (camera.getX() + camera.getWidth()) &&
            y >= (camera.getY() - height) &&
            y <= (camera.getY() + camera.getHeight())){
                const double viewx = x - camera.getX();
                const double viewy = y - camera.getY();
                camera.getWindow().rectangle(viewx, viewy, viewx+width, viewy+height, Graphics::makeColor(128,128,128));
        }
}

void ScriptObject::add(const std::string & what, const Runnable & runnable){
    scripts.insert(std::pair<std::string, Runnable>(what, runnable));
}
#endif
