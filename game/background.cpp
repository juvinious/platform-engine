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
scrollX(1),
scrollY(1){
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
                    ostringstream out;
                    out << "Background parse error, unknown type given: ";
                    out << type;
		    throw LoadException(__FILE__, __LINE__, out.str());
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
    switch (type){
        case Anim:
            if (animation != NULL){
                
            }
            break;
        case Tileset:
            tiles->act();
            break;
        default:
            break;
    }
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
        /* FIXME Remove ability to render single large bitmaps, use tiles exclusively */
        const int x = scrollX * camera.getX();
        const int y = scrollY * camera.getY();
        animation->drawRepeatable(x, y, camera.getWindow());
    }
}

void Background::drawTileset(const Camera & camera){
    tiles->draw(scrollX, scrollY, camera);
}
