#ifndef platformer_world_h
#define platformer_world_h

#include <string>
#include <vector>
#include <map>

class Bitmap;
namespace Filesystem{
class AbsolutePath;
}
class Token;

/*! Platformers world class */

namespace Platformer{
    
class Animation;
class Background;
class Camera;
    
class World{
    public:
	World(const Token *);
	virtual ~World();
	virtual void act();
	virtual void draw(const Bitmap &);
    protected:
	virtual void load(const Filesystem::AbsolutePath &);
	virtual void load(const Token *);
	//! Name
	std::string name;
	//! Resolution of map (upscaled/downscaled according to paintowns screensize)
	int resolutionX;
	int resolutionY;
	//! Dimensions of the entire map
	int dimensionsX;
	int dimensionsY;
	// Player info
	
	//! Cameras
	std::vector< Camera * > cameras;
	
	//! Animation map
	std::map< int, Animation *> animations;
	
	//! Backgrounds
	std::vector< Background *> backgrounds;
};
}
#endif
