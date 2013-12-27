#ifndef platformer_object_h
#define platformer_object_h

#include <vector>

class Token;

/*! Platformers object class */

namespace Platformer{

class Camera;
      
class Object{
public:
    Object();
    virtual ~Object();
    virtual void act();
    virtual void draw(const Camera &);
    
    void move(int x, int y);
    
    inline virtual void attachCamera(int id){
        this->attachedCamera = id;
        this->followed = true;
    }
    
    inline virtual int getAttachedCamera() const {
        return this->attachedCamera;
    }
    
    inline virtual void detachCamera(){
        this->followed = false;
    }
    
    inline virtual bool isFollowed(){
        return this->followed;
    }
    
    inline virtual int getX() const {
        return this->x;
    }
    
    inline virtual int getY() const {
        return this->y;
    }
    
    inline virtual int getWidth() const {
        return this->width;
    }
    
    inline virtual int getHeight() const {
        return this->height;
    }
    
    inline virtual void setCollided(bool collided){
        this->collided = collided;
    }
    
protected:
    /*! Camera Info */
    bool followed;
    int attachedCamera;
    
    /*! Object Info */
    int x;
    int y;
    int width;
    int height;
    int life;
    bool invincible;
    
    bool collided;
};

}
#endif
