#ifndef platformer_world_h
#define platformer_world_h

class Bitmap;
class Token;

/*! Platformers world class */

namespace Platformer{
    
class World{
    public:
	World();
	World(Token *);
	virtual ~World();
	virtual void act();
	virtual void draw(const Bitmap &);
    private:
};
}
#endif
