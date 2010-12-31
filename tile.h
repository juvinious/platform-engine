#ifndef platformer_tile_h
#define platformer_tile_h

#include <string>

class Bitmap;
class Token;

/*! Platformers tile class */

namespace Platformer{
    
class Tile{
    public:
	Tile();
	Tile(Token *);
	virtual ~Tile();
	virtual void act();
	virtual void draw(const Bitmap &);
    private:
	std::string animation;
	int width;
	int height;
	
	// Add attributes
};
}
#endif
