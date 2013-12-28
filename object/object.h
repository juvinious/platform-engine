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
    
    void set(int x, int y);
    
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
    
    inline void setVelocity(double x, double y){
        this->velocityX = x;
        this->velocityY = y;
    }
    
    inline void addVelocity(double x, double y){
        this->velocityX += x;
        this->velocityY += y;
    }
    
    inline void setVelocityX(double velocity){
        this->velocityX = velocity;
    }
    
    inline void setVelocityY(double velocity){
        this->velocityY = velocity;
    }
    
    inline double getVelocityX() const {
        return this->velocityX;
    }
    
    inline double getVelocityY() const {
        return this->velocityY;
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
    double velocityX;
    double velocityY;
    int life;
    bool invincible;
    
    bool hasCollided;
};

}
#endif
