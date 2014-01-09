#ifndef platformer_object_h
#define platformer_object_h

#include <string>
#include <vector>

#include "util/pointer.h"

class Token;

/*! Platformers object class */

namespace Platformer{

class Camera;

namespace Collisions{
class Map;
}
      
class Object{
public:
    Object();
    virtual ~Object();
    virtual void act(const Util::ReferenceCount<Collisions::Map>, std::vector< Util::ReferenceCount<Object> > &);
    virtual void draw(const Camera &);
    
    void set(double x, double y);
    
    void move(double x, double y);
    
    inline const int getID() const {
        return this->id;
    }
    
    inline virtual void setLabel(const std::string & label){
        this->label = label;
    }
    
    inline virtual const std::string & getLabel() const {
        return this->label;
    }
    
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
    
    inline virtual void setX(double x){
        this->x = x;
    }
    
    inline virtual double getX() const {
        return this->x;
    }
    
    inline virtual double getY() const {
        return this->y;
    }
    
    inline virtual void setY(double y){
        this->y = y;
    }
    
    inline virtual void setWidth(int w){
        this->width = w;
    }
    
    inline virtual int getWidth() const {
        return this->width;
    }
    
    inline virtual void setHeight(int h){
        this->height = h;
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
private: 
    /*! ID */
    const int id;
    
protected:
    
    /*! Label */
    std::string label;
    
    /*! Camera Info */
    bool followed;
    int attachedCamera;
    
    /*! Object Info */
    double x;
    double y;
    int width;
    int height;
    double velocityX;
    double velocityY;
};

}
#endif
