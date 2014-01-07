#include "script.h"

#include "platformer/game/camera.h"
#include "platformer/game/world.h"
#include "platformer/object/object.h"

#include "util/file-system.h"

#ifdef HAVE_PYTHON
#include <Python.h>

class TestObject : public Object{
public:
    TestObject(double x, double y, int width, int height):
    x(x),
    y(y)
    width(width),
    height(height),
    hasCollided(false){
    }
    virtual ~TestObject(){}

    void rectDraw(const Area & area, double portx, double porty, const Graphics::Bitmap & bmp, bool collision){
        const double viewx = (area.x > portx ? area.x - portx : portx - area.x);
        const double viewy = (area.y > porty ? area.y - porty : porty - area.y);
        
        bmp.rectangle(viewx, viewy, viewx+area.width, viewy+area.height, 
                                                 (collision ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));
    }
    
    void act(const Util::ReferenceCount<CollisionMap> collisionMap){
        
        class Collider : public CollisionBody{
        public:
            Collider(TestObject & object):
            object(object){
                area.x = object.getX();
                area.y = object.getY();
                area.width = object.getWidth();
                area.height = object.getHeight();
                velocityX = object.getVelocityX();
                velocityY = object.getVelocityY();
            }
            ~Collider(){}
            TestObject & object;
            
            void response(const CollisionInfo & info) const {
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

    void draw(const Camera & camera){
        if (x >= camera.getX() && 
            x <= (camera.getX() + camera.getWidth()) &&
            y >= camera.getY() &&
            y <= (camera.getY() + camera.getHeight())){
                Area area(x, y, width, height);
                rectDraw(area, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
        }
    }
    void setCollided(bool collided){
        this->hasCollided = collided;
    }
private:
    bool hasCollided;
};

static PyObject * createObject(PyObject *, PyObject * args){
    PyObject * world;
    double x = 0;
    double y = 0;
    int width = 0;
    int height = 0;

    if (PyArg_ParseTuple(args, "Oddii", &world, &x, &y, &width, &height)){
        Util::ReferenceCount<Object> object(new TestObject(x,y,width,height));
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
    Python(World * world):
    world(world){
        Py_Initialize();
        Py_InitModule("platformer", Methods);
    }
    
    virtual ~Python(){
        Py_Finalize();
    }
    
    void act(){
    }
    
    void render(const Camera & camera){
    }
    
    void registerAnimation(void *){
    }
    void registerObject(void *){
    }
    
    void loadScript(const std::string & module){
        PyObject * sysPath = PySys_GetObject("path");
        // FIXME Do not use a fixed location, for now make it data/platformer
        PyObject * path = PyString_FromString(Storage::instance().find(Filesystem::RelativePath("data/platformer/")).path());
        int result = PyList_Insert(sysPath, 0, path);
        PyObject * loadModule = PyImport_ImportModule(module.c_str());
        if (PyErr_Occured()){
            // TODO Throw an exception for now print
            PyErr_Print();
        }
    }

    World * world;
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
