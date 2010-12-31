#ifndef platformer_camera_h
#define platformer_camera_h

#include <string>


/*! Platformers camera class which provides the viewport to the user.
    It can be set to follow an in game object and/or free range */

namespace Platformer{
    
class Camera{
    public:
	Camera(int worldWidth, int worldHeight);
	virtual ~Camera();
	virtual void act();
	
	virtual void set(int x, int y);
	virtual void move(int x, int y);
	
	// Add Follow object or whatever
	
	// Add quake/shake
	
	// Other effects
	
	virtual inline const int getX() const{
	    return this->x;
	}
	
	virtual inline const int getY() const{
	    return this->y;
	}
	
    private:
	int worldWidth;
	int worldHeight;
	
	//! Right hand corner
	int x;
	int y;
	
	//! The window (or viewport) of the camera 
	int windowX;
	int windowY;
	int windowWidth;
	int windowHeight;
	
	double scrollSpeed;
	
	bool follow;
	double followVariance;
};
}
#endif
