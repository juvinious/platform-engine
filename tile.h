#ifndef platformer_tile_h
#define platformer_tile_h

#include <map>
#include <string>
#include "util/pointer.h"

namespace Graphics{
class Bitmap;
}
class Token;

/*! Platformers tile class */

namespace Platformer{
    
class Animation;
class Camera;
    
class Tile{
public:
    Tile();
    Tile(const Token *, std::map< std::string, Util::ReferenceCount<Animation> > &);
    virtual ~Tile();
    virtual void act();
    virtual void draw(int x, int y, const Graphics::Bitmap &);
    
    virtual void setAnimation(Util::ReferenceCount<Animation>);
    
    virtual inline const int getRow() const {
	return this->row;
    }
    
    virtual inline const int getColumn() const {
	return this->column;
    }
private:
    Util::ReferenceCount<Animation> animation;
    // Placement
    int row;
    int column;
    
    // Add attributes
};


//! Set up tile map
typedef std::map< int, std::map< int, Util::ReferenceCount<Tile> > > tileMap;

class TileManager{
public:
    TileManager(const Token *, std::map< std::string, Util::ReferenceCount<Animation> > &);
    virtual ~TileManager();
    
    virtual void act();
    virtual void draw(double scrollX, double scrollY, const Camera &);
protected:
    int tileX;
    int tileY;
    int dimensionsX;
    int dimensionsY;
    tileMap tiles;
};
}
#endif
