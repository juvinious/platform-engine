#ifndef platformer_camera_h
#define platformer_camera_h

#include "object.h"
#include "util/pointer.h"

#include <string>

namespace Graphics{
class Bitmap;
}
class Token;


/*! Platformers camera class which provides the viewport to the user.
    It can be set to follow an in game object and/or free range */

namespace Platformer{
    
class Camera{
    public:
        Camera(int resolutionX, int resolutionY, int worldWidth, int worldHeight, const Token *);
        virtual ~Camera();
        virtual void act();
        virtual void draw(const Graphics::Bitmap &);
        
        virtual void set(int x, int y);
        virtual void move(int x, int y);
        
        // Add Follow object or whatever
        virtual void followObject(Util::ReferenceCount<Object>);
        virtual void stopFollowing();
        
        virtual inline bool isFollowing() const { 
            return this->follow;
        }
        
        // Add quake/shake
        
        // Other effects
        
        virtual inline int getId() const {
            return this->id;
        }
        
        virtual inline int getX() const{
            return this->currentX;
        }
        
        virtual inline int getY() const{
            return this->currentY;
        }
        
        virtual int getWidth() const;
        virtual int getHeight() const;
        
        virtual inline int getViewportX() const{
            return this->windowX;
        }
        virtual inline int getViewportY() const{
            return this->windowY;
        }
        virtual inline int getViewportX2() const{
            return this->windowWidth;
        }
        virtual inline int getViewportY2() const{
            return this->windowHeight;
        }
        virtual inline int getViewportWidth() const{
            return (this->windowWidth - this->windowX);
        }
        virtual inline int getViewportHeight() const{
            return (this->windowHeight - this->windowY);
        }
        
        virtual inline Graphics::Bitmap & getWindow() const {
            return *this->window;
        }
        
    protected:
        //! ID
        int id;
    
        //! Screen Resolution of the game
        const int resolutionX;
        const int resolutionY;
        
        //! World Dimensions
        int worldWidth;
        int worldHeight;
        
        //! Right hand corner (location in which to move to)
        int x;
        int y;
        
        //! This is the current placement of the x and y
        int currentX;
        int currentY;
        
        //! The window (or viewport) of the camera 
        int windowX;
        int windowY;
        int windowWidth;
        int windowHeight;
        
        //! Window buffer
        Util::ReferenceCount<Graphics::Bitmap> window;
        
        double scrollSpeed;
        double currentXSpeed;
        double currentYSpeed;
        double velocity;
        
        bool follow;
        double followVariance;
        Util::ReferenceCount<Object> object;
        
        void checkBounds();
};
}
#endif
