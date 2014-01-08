#ifdef HAVE_PYTHON
#include <Python.h>
#endif 

#include "script.h"
#include "object.h"

#include "platformer/game/camera.h"
#include "platformer/game/world.h"
#include "platformer/object/object.h"
#include "platformer/game/collision-map.h"

#include "util/file-system.h"
#include "util/funcs.h"

#ifdef HAVE_PYTHON
/*
class PyTestObject : public Platformer::Object{
public:
    PyTestObject(double x, double y, int width, int height):
    hasCollided(false),
    ticks(0){
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    virtual ~PyTestObject(){}

    void rectDraw(const Platformer::Area & area, double portx, double porty, const Graphics::Bitmap & bmp, bool collision){
        const double viewx = area.x - portx;
        const double viewy = area.y - porty;
        
        bmp.rectangle(viewx, viewy, viewx+area.width, viewy+area.height, 
                                                 (collision ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));
    }
    
    void act(const Util::ReferenceCount<Platformer::CollisionMap> collisionMap, std::vector< Util::ReferenceCount<Object> > & objects){
        
        if (ticks < 60){
            ticks++;
        } else {
            switch (Util::rnd(5)){
                case 0:
                    velocityX += 2;
                    break;
                case 1:
                    velocityX -= 2;
                    break;
                case 2:
                    velocityY += .02;
                    break;
                case 3:
                default:
                    velocityY -= 3.5;
                    break;
            }
            ticks = 0;
        }
        
        class Collider : public Platformer::CollisionBody{
        public:
            Collider(PyTestObject & object):
            object(object){
                area.x = object.getX();
                area.y = object.getY();
                area.width = object.getWidth();
                area.height = object.getHeight();
                velocityX = object.getVelocityX();
                velocityY = object.getVelocityY();
            }
            ~Collider(){}
            PyTestObject & object;
            
            void response(const Platformer::CollisionInfo & info) const {
                bool collided = false;
                if (info.top){
                    object.setVelocityY(0);
                    collided = true;
                }
                if (info.bottom){
                    object.setVelocityY(0);
                    collided = true;
                }
                if (info.left){
                    object.setVelocityX(0);
                    collided = true;
                }
                if (info.right){
                    object.setVelocityX(0);
                    collided = true;
                }
                object.setCollided(collided);
            }
            
            void noCollision() const {
                object.setCollided(false);
            }
        };
        
        Collider collider(*this);
        collisionMap->collides(collider);
        
        x += velocityX;
        y += velocityY;
    }

    void draw(const Platformer::Camera & camera){
        if (x >= (camera.getX() - width) && 
            x <= (camera.getX() + camera.getWidth()) &&
            y >= (camera.getY() - height) &&
            y <= (camera.getY() + camera.getHeight())){
                Platformer::Area area(x, y, width, height);
                rectDraw(area, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
        }
    }
    void setCollided(bool collided){
        this->hasCollided = collided;
    }
private:
    bool hasCollided;
    int ticks;
};*/

static PyObject * createObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    char * module;
    char * initFunction;

    if (PyArg_ParseTuple(args, "Oss", &worldObject, &module, &initFunction)){
        Platformer::World * world = (Platformer::World*) PyCapsule_GetPointer(worldObject, "world");
        //Util::ReferenceCount<Platformer::Object> object(new PyTestObject(x,y,width,height));
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
        
        // Script execute function
        PyObject * execute = PyObject_GetAttrString(loadModule, func.c_str());
        if (execute == NULL){
            PyErr_Print();
        }
        
        PyObject * worldObject = PyCapsule_New((void *) world, "world", NULL);
        if (worldObject == NULL){
            PyErr_Print();
        }
        
        // Execute the script passing world in
        PyObject * result = PyObject_CallFunction(execute, (char*) "(O)", worldObject);
        if (result == NULL){
            PyErr_Print();
        }
        
        /*Py_DECREF(sysPath);
        Py_DECREF(path);*/
        Py_DECREF(loadModule);
        Py_DECREF(execute);
        Py_DECREF(worldObject);
        Py_DECREF(result);
    }

    Platformer::World * world;
};

#endif

Util::ReferenceCount<Platformer::Scriptable> Platformer::Scriptable::getInstance(World * world){
#ifdef HAVE_PYTHON
    return Util::ReferenceCount<Platformer::Scriptable>(new Python(world));
#else
    // No script support
    class NoScript : public Platformer::Scriptable{
    public:
        NoScript(){
        }
        virtual ~NoScript(){
        }
        
        void act(){
        }
        
        void render(const Camera &){
        }
        void registerAnimation(void *){
        }
    };
    return Util::ReferenceCount<Platformer::Scriptable>(new NoScript());
#endif
}
