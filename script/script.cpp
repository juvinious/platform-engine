#include "script.h"

#include "platformer/game/camera.h"
#include "platformer/game/world.h"
#include "platformer/object/object.h"
#include "platformer/game/collision-map.h"

#include "util/file-system.h"
#include "util/funcs.h"

#ifdef HAVE_PYTHON
#include <Python.h>

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
        const double viewx = (area.x > portx ? area.x - portx : portx - area.x);
        const double viewy = (area.y > porty ? area.y - porty : porty - area.y);
        
        bmp.rectangle(viewx, viewy, viewx+area.width, viewy+area.height, 
                                                 (collision ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));
    }
    
    void act(const Util::ReferenceCount<Platformer::CollisionMap> collisionMap){
        
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
                    velocityY -= 2;
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
        if (x >= camera.getX() && 
            x <= (camera.getX() + camera.getWidth()) &&
            y >= camera.getY() &&
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
};

static PyObject * createObject(PyObject *, PyObject * args){
    PyObject * worldObject;
    double x = 0;
    double y = 0;
    int width = 0;
    int height = 0;

    if (PyArg_ParseTuple(args, "Oddii", &worldObject, &x, &y, &width, &height)){
        Platformer::World * world = (Platformer::World*) PyCObject_AsVoidPtr(worldObject);
        Util::ReferenceCount<Platformer::Object> object(new PyTestObject(x,y,width,height));
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
    }
    
    virtual ~Python(){
        Py_Finalize();
    }
    
    void act(){
    }
    
    void render(const Platformer::Camera & camera){
    }
    
    void registerAnimation(void *){
    }
    void registerObject(void *){
    }
    
    void loadScript(const std::string & module){
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
        PyObject * execute = PyObject_GetAttrString(loadModule, "run");
        if (execute == NULL){
            PyErr_Print();
        }
        
        // Create world object
        PyObject * worldObject = PyCObject_FromVoidPtr((void*) world, NULL);
        if (worldObject == NULL){
            PyErr_Print();
        }
        
        //PyObject * worldObject = PyCapsule_New((void *) &world
        
        // Execute the script passing world in
        PyObject * result = PyObject_CallFunction(execute, (char*) "(O)", worldObject);
        if (result == NULL){
            PyErr_Print();
        } else {
            Py_DECREF(result);
        }
        Py_DECREF(execute);
        Py_DECREF(worldObject);
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
        void registerObject(void *){
        }
        void loadScript(const std::string &){
        }
    };
    return Util::ReferenceCount<Platformer::Scriptable>(new NoScript());
#endif
}
