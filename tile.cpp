#include "tile.h"
#include "animation.h"
#include "camera.h"

#include "util/bitmap.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token.h"

using namespace std;
using namespace Platformer;

Tile::Tile(){
}

Tile::Tile(Token * token, std::map< int, Animation *> & animations){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "image"){
		int num;
		tok->view() >> num;
		animation = animations[num];
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

void Tile::draw(const Bitmap & bmp){
}

void Tile::setAnimation(Animation * anim){
}


TileManager::TileManager(const Token * token, std::map< int, Animation *> & animations):
dimensionsX(0),
dimensionsY(0){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "dimensions"){
		tok->view() >> dimensionsX >> dimensionsY;
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

void TileManager::draw(const Camera & camera){
}


