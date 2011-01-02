#include "background.h"
#include "util/bitmap.h"
#include "util/token.h"
#include "camera.h"
#include "tile.h"

#include "util/debug.h"
#include "util/load_exception.h"

using namespace std;
using namespace Platformer;

Background::Background(const Token * token, const std::map< int, Animation *> & animations):
type(Tileset),
image(0){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "type"){
		std::string typeTemp;
                // get the name
                tok->view() >> typeTemp;
		if (typeTemp == "image"){
		    type = Image;
		} else if (typeTemp == "tileset"){
		    type = Tileset;
		} else {
		    throw LoadException(__FILE__, __LINE__, "Background parse error, unknown type given: " + type);
		}
            } else {
                Global::debug( 3 ) << "Unhandled World attribute: "<< endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "World parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

Background::~Background(){
}

void Background::act(){
}

void Background::draw(const Camera & camera, const Bitmap & bmp){
}