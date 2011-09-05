#include "background.h"

#include "animation.h"
#include "camera.h"
#include "tile.h"

#include "util/bitmap.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token.h"

using namespace std;
using namespace Platformer;

Background::Background(const Token * token, std::map< std::string, Util::ReferenceCount<Animation> > & animations):
type(Tileset),
tiles(0),
animation(0),
scrollX(0),
scrollY(0){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "type"){
		std::string typeTemp;
                // get the name
                tok->view() >> typeTemp;
		if (typeTemp == "animation"){
		    type = Anim;
		} else if (typeTemp == "tileset"){
		    type = Tileset;
		} else {
		    throw LoadException(__FILE__, __LINE__, "Background parse error, unknown type given: " + type);
		}
            } else if (*tok == "animation"){
		std::string name;
		tok->view() >> name;
		animation = animations[name];
	    } else if (*tok == "tileset"){
		tiles = new TileManager(tok, animations);
	    } else if (*tok == "scroll-x"){
		tok->view() >> scrollX;
	    } else if (*tok == "scroll-y"){
		tok->view() >> scrollY;
	    } else {
                Global::debug( 3 ) << "Unhandled World attribute: "<< endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Background parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

Background::~Background(){
}

void Background::act(){
}

void Background::draw(const Camera & camera){
    switch (type){
	case Anim:
	    drawAnimation(camera);
	    break;
	case Tileset:
	    drawTileset(camera);
	    break;
	default:
	    break;
    }
}

void Background::drawAnimation(const Camera & camera){
    if (animation != NULL){
        const int x = scrollX * camera.getX();
        const int y = scrollY * camera.getY();
        animation->drawRepeatable(x,y,camera.getWindow());
    }
}

void Background::drawTileset(const Camera & camera){
    tiles->draw(camera);
}