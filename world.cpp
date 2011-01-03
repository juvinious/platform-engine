#include "world.h"

#include "animation.h"
#include "background.h"
#include "camera.h"

#include "util/bitmap.h"
#include "util/debug.h"
#include "util/file-system.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/tokenreader.h"

using namespace std;
using namespace Platformer;

World::World(const Token * token):
resolutionX(0),
resolutionY(0),
dimensionsX(0),
dimensionsY(0){
    if ( *token != "world" ){
        throw LoadException(__FILE__, __LINE__, "Not world.");
    }
    
    if (token->numTokens() == 1){
        std::string temp;
        token->view() >> temp;
        load(Filesystem::find(Filesystem::RelativePath(temp)));
    } else {
        load(token);
    }
}

void World::load(const Filesystem::AbsolutePath & filename){
    // Load up tokenizer
    try{
        Global::debug(1,"World") << "Loading world " << filename.path() << endl;
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading World");
    }
}

void World::load(const Token * token){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "name"){
                // get the name
                tok->view() >> name;
		Global::debug(0, "Platformer") << "Loading :" << name << endl;
            } else if (*tok == "resolution"){
                // Get the resolution of the world
                tok->view() >> resolutionX >> resolutionY;
            } else if (*tok == "dimensions"){
                // Get the dimensions of the world
                tok->view() >> dimensionsX >> dimensionsY;
            } else if (*tok == "players"){
		// Handle player info eventually
            } else if (*tok == "camera"){
                // Handle camera info
                Camera * camera = new Camera(resolutionX, resolutionY, dimensionsX, dimensionsY, tok);
		cameras[camera->getId()] = camera;
            } else if (*tok == "anim"){
		Animation * animation = new Animation(tok);
		animations[animation->getId()] = animation;
	    } else if (*tok == "background"){
		Background * background = new Background(tok, animations);
		backgrounds.push_back(background);
	    } else {
                Global::debug( 3 ) << "Unhandled World attribute: "<<endl;
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

World::~World(){
    for (std::map< int, Camera *>::iterator i = cameras.begin(); i != cameras.end(); ++i){
        if (i->second){
            delete i->second;
        }
    }
    
    for (std::map< int, Animation *>::iterator i = animations.begin(); i != animations.end(); ++i){
        if (i->second){
            delete i->second;
        }
    }
    
    for (std::vector< Background *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
}

void World::act(){
    cameras[0]->act();
    
    for (std::map< int, Animation *>::iterator i = animations.begin(); i != animations.end(); ++i){
	Animation * animation = i->second;
        if (animation){
            animation->act();
        }
    }
    
    for (std::vector< Background *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
	Background * background = *i;
	if (background){
	    background->act();
	}
    }
}

void World::draw(const Bitmap & bmp){
    // FIXME Must correct so that cameras are handled properly
    for (std::vector< Background *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
	Background * background = *i;
	if (background){
	    background->draw(*cameras[0]);
	}
    }
    Bitmap temp = Bitmap::temporaryBitmap(resolutionX, resolutionY);
    cameras[0]->draw(temp);
    temp.Stretch(bmp);
}

void World::moveCamera(int x, int y){
    // FIXME this needs to change to accomodate the cameras accordingly
    cameras[0]->move(x,y);
}

const Camera & World::getCamera(int number){
    // FIXME this needs to change to accomodate the cameras accordingly
    return *cameras[number];
}