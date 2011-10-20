#include "world.h"

#include "animation.h"
#include "background.h"
#include "camera.h"
#include "platformer/script/script.h"

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
        load(Storage::instance().find(Filesystem::RelativePath(temp)));
    } else {
        load(token);
    }
    
    //! Setup script engine
    scriptEngine = Scriptable::getScriptable();
    scriptEngine->init(this);
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
		Global::debug(0, "Platformer") << "Loading: " << name << endl;
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
            } else if (*tok == "animation"){
		Util::ReferenceCount<Animation> animation(new Animation(tok));
		animations[animation->getId()] = animation;
	    } else if (*tok == "background"){
		Util::ReferenceCount<Background> background(new Background(tok, animations));
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
}

void World::act(){
    for (std::map< int, Util::ReferenceCount<Camera> >::iterator c = cameras.begin(); c != cameras.end(); ++c){
        Util::ReferenceCount<Camera> camera = c->second;
        camera->act();
    }
    
    for (std::map< std::string, Util::ReferenceCount<Animation> >::iterator i = animations.begin(); i != animations.end(); ++i){
        Util::ReferenceCount<Animation> animation = i->second;
        if (animation != NULL){
            animation->act();
        }
    }
    
    for (std::vector< Util::ReferenceCount<Background> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
        Util::ReferenceCount<Background> background = *i;
        if (background != NULL){
            background->act();
        }
    }
    
    scriptEngine->act();
}

void World::draw(const Graphics::Bitmap & bmp){
    // Go through all cameras
    for (std::map< int, Util::ReferenceCount<Camera> >::iterator c = cameras.begin(); c != cameras.end(); ++c){
        Util::ReferenceCount<Camera> camera = c->second;
        for (std::vector< Util::ReferenceCount<Background> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
            Util::ReferenceCount<Background> background = *i;
            if (background != NULL){
                background->draw(*camera);
            }
        }
        // Render objects to camera
        
        // Render scriptable items to camera
        scriptEngine->render(*camera);
        
        // Render camera to bmp
        camera->draw(bmp);
    }
}

void World::moveCamera(int id, int x, int y){
    std::map< int, Util::ReferenceCount<Camera> >::iterator found = cameras.find(id);
    if (found != cameras.end()){
        found->second->move(x,y);
    }
}

Util::ReferenceCount<Camera> World::getCamera(int id){
    std::map< int, Util::ReferenceCount<Camera> >::iterator found = cameras.find(id);
    if (found != cameras.end()){
        return found->second;
    }
    
    return Util::ReferenceCount<Camera>(NULL);
}
