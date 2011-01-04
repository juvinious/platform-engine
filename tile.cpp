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

Tile::Tile(const Token * token, std::map< std::string, Animation *> & animations):
row(0),
column(0){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "animation"){
		std::string name;
		tok->view() >> name;
		animation = animations[name];
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


TileManager::TileManager(const Token * token, std::map< std::string, Animation *> & animations):
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
		const int row = tile->getRow();
		const int col = tile->getColumn();
		// Do first one
		tiles[tile->getRow()][tile->getColumn()] = tile;
		// Check repeatables
		int times = 0;
		if (tok->match("_/repeat-up", times)){
		    for (int i = row-1; i >= (row - times); --i){
			if (i < 0){
			    break;
			}
			tiles[i][tile->getColumn()] = tile;
		    }
		}
		if (tok->match("_/repeat-down", times)){
		    for (int i = row+1; i <= row + times; ++i){
			if (i > dimensionsY){
			    break;
			}
			tiles[i][tile->getColumn()] = tile;
		    }
		}
		if (tok->match("_/repeat-left", times)){
		    for (int i = col-1; i >= (col - times); --i){
			if (i < 0){
			    break;
			}
			tiles[tile->getRow()][i] = tile;
		    }
		}
		if (tok->match("_/repeat-right", times)){
		    for (int i = col+1; i <= col + times; ++i){
			if (i > dimensionsX){
			    break;
			}
			tiles[tile->getRow()][i] = tile;
		    }
		}
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
    // Render tiles 
    const int x = (int)((camera.getX())/tileX);
    const int y = (int)((camera.getY())/tileY);
    const int w = x + (int)(camera.getWindow().getWidth()/tileX) + 1;
    const int h = y + (int)(camera.getWindow().getHeight()/tileY);
    int posy = y * tileY - camera.getY();
    for(int row = y; row < h; ++row){
	int posx = x * tileX - camera.getX();
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


