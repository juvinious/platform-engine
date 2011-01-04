#ifndef platformer_background_h
#define platformer_background_h

#include <string>
#include <map>

class Bitmap;
class Token;

/*! Platformers Background class which can be either backgrounds or foregrounds.
    Each consist of a tilemap or singe image */

namespace Platformer{

class Animation;
class Camera;
class TileManager;
    
class Background{
    public:
	Background(const Token *, std::map< std::string, Animation *> &);
	virtual ~Background();
	virtual void act();
	virtual void draw(const Camera &);
	enum Type { 
	    Anim=0,
	    Tileset,
	};
    protected:
	//! Type of background (uses tiles or an image)
	Type type;
	//! Tileset
	TileManager * tiles;
	//! Image
	Animation * animation;
	
	//! X Velocity - Used to determine the X offset from where the camera is set (only applicable to animations, tiles don't move)
	double scrollX;
	//! Y Velocity - Used to determine the X offset from where the camera is set (only applicable to animations, tiles don't move)
	double scrollY;
	
	//! Render Animation
	void drawAnimation(const Camera &);
	
	//! Render tileset
	void drawTileset(const Camera &);
};
}
#endif
