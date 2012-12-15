#include "tile.h"
#include "animation.h"
#include "camera.h"
#include "platformer/object/object.h"

#include "util/graphics/bitmap.h"
#include "util/debug.h"
#include "util/exceptions/load_exception.h"
#include "util/token.h"

using namespace std;
using namespace Platformer;

Tile::Tile():
row(0),
column(0),
destructable(false),
x(0),
y(0),
width(0),
height(0),
life(0),
weakPoint(None){
}

Tile::Tile(const Token * token, std::map< std::string, Util::ReferenceCount<Animation> > & animations):
row(0),
column(0),
destructable(false),
x(0),
y(0),
width(0),
height(0),
life(0),
weakPoint(None){
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
    if (animation != NULL){
        // FIXME Do something useful
    }
}

void Tile::draw(int x, int y, const Graphics::Bitmap & bmp){
    if (animation != NULL){
        animation->draw(x, y, bmp);
    }
}

Tile::Side Tile::collides(const Object & object){
    return None;
}

Util::ReferenceCount<Object> Tile::toObject(){
    return Util::ReferenceCount<Object>(NULL);
}

void Tile::setAnimation(Util::ReferenceCount<Animation> anim){
}

TileManager::TileManager(const Token * token, std::map< std::string, Util::ReferenceCount<Animation> > & animations):
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
                Util::ReferenceCount<Tile> tile(new Tile(tok, animations));
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
    // Set tile widths and positioning
    for (std::map< int, std::map< int, Util::ReferenceCount<Tile> > >::iterator row = tiles.begin(); row != tiles.end(); ++row){
        for (std::map< int, Util::ReferenceCount<Tile> >::iterator column = row->second.begin(); column != row->second.end(); ++column){
            Util::ReferenceCount<Tile> tile = column->second;
            if (tile != NULL){
                tile->setSize(tileX, tileY);
                tile->setX(tile->getColumn() * tileX);
                tile->setX(tile->getRow() * tileY);
            }
        }
    }
}

TileManager::~TileManager(){
    
}

void TileManager::act(){
    for (std::map< int, std::map< int, Util::ReferenceCount<Tile> > >::iterator row = tiles.begin(); row != tiles.end(); ++row){
        for (std::map< int, Util::ReferenceCount<Tile> >::iterator column = row->second.begin(); column != row->second.end(); ++column){
            Util::ReferenceCount<Tile> tile = column->second;
            if (tile != NULL){
                tile->act();
            }
        }
    }
}

void TileManager::draw(double scrollX, double scrollY, const Camera & camera){
    // Render tiles 
    const int cameraX = camera.getX() * scrollX;
    const int cameraY = camera.getY() * scrollY;
    const int x = (int)((cameraX)/tileX);
    const int y = (int)((cameraY)/tileY);
    const int w = x + (int)(camera.getWidth()/tileX) + 2;
    const int h = y + (int)(camera.getHeight()/tileY) + 2;
    int posy = y * tileY - cameraY;
    for(int row = y; row < h; ++row){
        int posx = x * tileX - cameraX;
        for(int column = x; column < w; ++column){
            Util::ReferenceCount<Tile> tile = tiles[row][column];
            if (tile != NULL){
                tile->draw(posx, posy, camera.getWindow());
            }
            posx+=tileX;
        }
        posy+=tileY;
    }
}


