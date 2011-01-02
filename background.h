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
class Tile;
    
typedef std::map< int, std::map< int, Tile *> > tileMap;
    
class Background{
    public:
	Background(const Token *, const std::map< int, Animation *> &);
	virtual ~Background();
	virtual void act();
	virtual void draw(const Camera &, const Bitmap &);
	enum Type { 
	    Image=0,
	    Tileset,
	};
    private:
	//! Type of background (uses tiles or an image)
	Type type;
	//! Tileset
	tileMap tiles;
	//! Image
	Bitmap * image;
	//! X Velocity - Used to determine the X offset from where the camera is set
	double scrollX;
	//! Y Velocity - Used to determine the X offset from where the camera is set
	double scrollY;
};
}
#endif
