#ifdef HAVE_PYTHON
#include <Python.h>
#include "object.h"
#include "platformer/game/camera.h"
#include "platformer/game/collision-map.h"

#include "util/graphics/bitmap.h"
#include "util/file-system.h"
#include "util/funcs.h"

using namespace Platformer;

static PyObject * getID(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        return Py_BuildValue("i", obj->getID());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getLabel(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("s", obj->getLabel().c_str());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setLabel(PyObject *, PyObject * args){
    PyObject * charPointer;
    char * label;
    if (PyArg_ParseTuple(args, "Os", &charPointer, &label)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setLabel(label);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getX(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("d", obj->getX());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setX(PyObject *, PyObject * args){
    PyObject * charPointer;
    double x = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &x)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setX(x);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getY(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("d", obj->getY());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setY(PyObject *, PyObject * args){
    PyObject * charPointer;
    double y = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &y)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setY(y);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getWidth(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("i", obj->getWidth());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setWidth(PyObject *, PyObject * args){
    PyObject * charPointer;
    int width = 0;
    if (PyArg_ParseTuple(args, "Oi", &charPointer, &width)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setWidth(width);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getHeight(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("i", obj->getHeight());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setHeight(PyObject *, PyObject * args){
    PyObject * charPointer;
    int height = 0;
    if (PyArg_ParseTuple(args, "Oi", &charPointer, &height)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setHeight(height);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getArea(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("{sdsdsisi}", 
                                "x", obj->getX(), 
                                "y", obj->getY(),
                                "width", obj->getWidth(),
                                "height", obj->getHeight());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getVelocityX(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("d", obj->getVelocityX());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setVelocityX(PyObject *, PyObject * args){
    PyObject * charPointer;
    double velocity = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &velocity)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setVelocityX(velocity);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * getVelocityY(PyObject *, PyObject * args){
    PyObject * charPointer;
    if (PyArg_ParseTuple(args, "O", &charPointer)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));    
        return Py_BuildValue("d", obj->getVelocityY());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * setVelocityY(PyObject *, PyObject * args){
    PyObject * charPointer;
    double velocity = 0;
    if (PyArg_ParseTuple(args, "Od", &charPointer, &velocity)){
        Object * obj = reinterpret_cast<Object*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->setVelocityY(velocity);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * addScriptAction(PyObject *, PyObject * args){
    PyObject * charPointer;
    char * action;
    char * module;
    char * function;
    if (PyArg_ParseTuple(args, "Osss", &charPointer, &action, &module, &function)){
        ScriptObject * obj = reinterpret_cast<ScriptObject*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->add(action, Script::Runnable(module, function));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * addAction(PyObject *, PyObject * args){
    PyObject * charPointer;
    char * action;
    PyObject * func;
    if (PyArg_ParseTuple(args, "OsO", &charPointer, &action, &func)){
        Script::AutoRef function(func);
        
        ScriptObject * obj = reinterpret_cast<ScriptObject*>(PyCapsule_GetPointer(charPointer, "object"));
        obj->add(action, Script::Runnable(function));
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
    {"getArea", getArea, METH_VARARGS, "Get Area {x:#,y:#,width:#,height:#}."},
    {"getVelocityX", getVelocityX, METH_VARARGS, "Get x velocity."},
    {"setVelocityX", setVelocityX, METH_VARARGS, "Set x velocity."},
    {"getVelocityY", getVelocityY, METH_VARARGS, "Get y velocity."},
    {"setVelocityY", setVelocityY, METH_VARARGS, "Set y velocity."},
    {"addScriptAction", addScriptAction, METH_VARARGS, "Add an action from a script."},
    {"addAction", addAction, METH_VARARGS, "Add an action from a function directly."},
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
    Script::Runnable init(initModule, initFunction);
    PyObject * self = PyCapsule_New((void *) this, "object", NULL);
    if (self == NULL){
        PyErr_Print();
    }
    
    // Execute init
    Script::AutoRef function = init.getFunction();
    PyObject * result = PyObject_CallFunction(function.getObject(), (char*) "(O)", self);
    if (result == NULL){
        PyErr_Print();
    }
    Py_DECREF(self);
    Py_DECREF(result);
}

ScriptObject::~ScriptObject(){
}

void ScriptObject::act(const Util::ReferenceCount<Platformer::CollisionMap> collisionMap, std::vector< Util::ReferenceCount<Object> > & objects){
    Script::AutoRef self(PyCapsule_New((void *) this, "object", NULL));
    if (self.getObject() == NULL){
        PyErr_Print();
    } else {
        // Act
        Script::RunMap::iterator act = scripts.find("act-objects");
        if (act != scripts.end()){
            const Script::Runnable actFunction = act->second;
            for (std::vector< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); i++){
                Script::AutoRef function = actFunction.getFunction();
                PyObject * object = PyCapsule_New((void *) (*i).raw(), "object", NULL);
                if (object == NULL){
                    PyErr_Print();
                }
                // run act-objects
                PyObject * result = PyObject_CallFunction(function.getObject(), (char *)"(OO)", self.getObject(), object);
                if (result == NULL){
                    PyErr_Print();
                }
                Py_DECREF(object);
                Py_XDECREF(result);
            }
        }
        
        class Collider : public CollisionBody{
        public:
            Collider(ScriptObject & object, Script::AutoRef self):
            object(object),
            self(self){
                area.x = object.getX();
                area.y = object.getY();
                area.width = object.getWidth();
                area.height = object.getHeight();
                velocityX = object.getVelocityX();
                velocityY = object.getVelocityY();
            }
            ~Collider(){
            }
            ScriptObject & object;
            Script::AutoRef self;
            void response(const CollisionInfo & info) const {
                Script::RunMap::iterator hit = object.scripts.find("collision-map");
                if (hit != object.scripts.end()){
                    const Script::Runnable collision = hit->second;
                    Script::AutoRef function = collision.getFunction();
                    PyObject * result = PyObject_CallFunction(function.getObject(), (char *)"(O{sisisisi}{sdsdsisi})", 
                                        self.getObject(),
                                        "top",info.top,
                                        "bottom",info.bottom,
                                        "left",info.left,
                                        "right",info.right,
                                        "x",info.area.x,
                                        "y",info.area.y,
                                        "width",info.area.width,
                                        "height",info.area.height);
                    if (result == NULL){
                        PyErr_Print();
                    }
                    Py_XDECREF(result);
                }
            }
        };
        
        Collider collider(*this, self);
        collisionMap->collides(collider);
        
        act = scripts.find("act");
        if (act != scripts.end()){
            const Script::Runnable actFunction = act->second;
            // run act
            Script::AutoRef function = actFunction.getFunction();
            PyObject * result = PyObject_CallFunction(function.getObject(), (char *)"(O)", self.getObject());
            if (result == NULL){
                PyErr_Print();
            }
            Py_XDECREF(result);
        }
    }
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

void ScriptObject::add(const std::string & what, const Script::Runnable & runnable){
    scripts.insert(std::pair<std::string, Script::Runnable>(what, runnable));
}
#endif
