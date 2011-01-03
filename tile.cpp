#include "tile.h"
#include "animation.h"
#include "camera.h"

#include "util/bitmap.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token.h"

using namespace std;
using namespace Platformer;

Tile::Tile():
row(0),
column(0){
}

Tile::Tile(const Token * token, std::map< int, Animation *> & animations):
row(0),
column(0){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "image"){
		int num;
		tok->view() >> num;
		animation = animations[num];
            } else if (*tok == "position"){
		tok->view() >> row >> column;
	    } else {
                Global::debug( 3 ) << "Unhandled Tile attribute: "<< endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Tile parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

Tile::~Tile(){
}

void Tile::act(){
}

void Tile::draw(int x, int y, const Bitmap & bmp){
    if (animation){
	animation->draw(x, y, bmp);
    }
}

void Tile::setAnimation(Animation * anim){
}


TileManager::TileManager(const Token * token, std::map< int, Animation *> & animations):
tileX(0),
tileY(0),
dimensionsX(0),
dimensionsY(0){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "tile-size"){
		tok->view() >> tileX >> tileY;
            } else if (*tok == "dimensions"){
		tok->view() >> dimensionsX >> dimensionsY;
            } else if (*tok == "tile"){
		Tile * tile = new Tile(tok, animations);
		tiles[tile->getRow()][tile->getColumn()] = tile;
	    } else {
                Global::debug( 3 ) << "Unhandled Tilemap attribute: "<< endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Tilemap parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

TileManager::~TileManager(){
    for (tileMap::iterator i = tiles.begin(); i != tiles.end(); ++i){
	for (std::map< int, Tile *>::iterator j = i->second.begin(); j != i->second.end(); j++){
	    if (j->second){
		delete j->second;
	    }
	}
    }
}

void TileManager::act(){
}

void TileManager::draw(int scrollx, int scrolly, const Camera & camera){
    const int x = (int)((scrollx * camera.getX())/tileX);
    const int y = (int)((scrolly * camera.getY())/tileY);
    const int w = (int)((x + camera.getWindow().getWidth())/tileX);
    const int h = (int)((y + camera.getWindow().getHeight())/tileY);
    int posx = x * tileX;
    int posy = y * tileY;
    for(int row = y; row < h; ++row){
	for(int column = x; column < w; ++column){
	    Tile * tile = tiles[row][column];
	    if (tile){
		tile->draw(posx, posy, camera.getWindow());
	    }
	    posx+=tileX;
	}
	posy+=tileY;
    }
}


