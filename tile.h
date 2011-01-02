#ifndef platformer_tile_h
#define platformer_tile_h

#include <map>
#include <string>

class Bitmap;
class Token;

/*! Platformers tile class */

namespace Platformer{
    
class Animation;
class Camera;
    
class Tile{
public:
    Tile();
    Tile(Token *, std::map< int, Animation *> &);
    virtual ~Tile();
    virtual void act();
    virtual void draw(const Bitmap &);
    
    virtual void setAnimation(Animation *);
private:
    Animation * animation;
    int width;
    int height;
    
    // Add attributes
};


//! Set up tile map
typedef std::map< int, std::map< int, Tile *> > tileMap;

class TileManager{
public:
    TileManager(const Token *, std::map< int, Animation *> &);
    virtual ~TileManager();
    
    virtual void act();
    virtual void draw(const Camera &);
protected:
    int dimensionsX;
    int dimensionsY;
    tileMap tiles;
};
}
#endif
