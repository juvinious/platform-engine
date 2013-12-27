#ifndef platformer_object_h
#define platformer_object_h

#include <vector>

#include "platformer/game/collision-map.h"

class Token;

/*! Platformers object class */

namespace Platformer{

class Camera;
      
class Object : public CollisionBody{
public:
    Object();
    virtual ~Object();
    virtual void act();
    virtual void draw(const Camera &);
    
    void set(int x, int y);
    
    void move(int x, int y);
    
    void collided(const CollisionInfo &);
    
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
    
    inline virtual void setX(int x){
        this->x = x;
    }
    
    inline virtual int getX() const {
        return this->x;
    }
    
    inline virtual int getY() const {
        return this->y;
    }
    
    inline virtual void setY(int y){
        this->y = y;
    }
    
    inline virtual int getWidth() const {
        return this->width;
    }
    
    inline virtual int getHeight() const {
        return this->height;
    }
    
    inline virtual void setCollided(bool collided){
        this->hasCollided = collided;
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
    
    bool hasCollided;
};

}
#endif
