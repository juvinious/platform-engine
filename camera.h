#ifndef platformer_camera_h
#define platformer_camera_h

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
	
	// Add quake/shake
	
	// Other effects
	
	virtual inline const int getId() const {
	    return this->id;
	}
	
	virtual inline const int getX() const{
	    return this->currentX;
	}
	
	virtual inline const int getY() const{
	    return this->currentY;
	}
	
	virtual inline const Graphics::Bitmap & getWindow() const {
	    return *this->window;
	}
	
    protected:
	//! ID
	int id;
    
    const int resolutionX;
    const int resolutionY;
	
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
        Graphics::Bitmap * window;
	
	double scrollSpeed;
	double currentXSpeed;
	double currentYSpeed;
	double velocity;
	
	bool follow;
	double followVariance;
	
	void checkBounds();
};
}
#endif
