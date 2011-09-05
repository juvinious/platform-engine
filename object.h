#ifndef platformer_object_h
#define platformer_object_h

#include <vector>

class Token;

/*! Platformers object class */

namespace Platformer{
      
class Object{
    public:
	Object();
	virtual ~Object();
	virtual void act();
        
        inline virtual void attachCamera(int id){
            this->attachedCamera = id;
            this->followed = true;
        }
        
        inline virtual void detachCamera(){
            this->followed = false;
        }
        
        inline virtual bool isFollowed(){
            return this->followed;
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
};
}
#endif
