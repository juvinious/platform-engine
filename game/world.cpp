#include "world.h"

#include "animation.h"
#include "background.h"
#include "camera.h"
#include "collision-map.h"
#include "platformer/script/script.h"

#include "util/graphics/bitmap.h"
#include "util/debug.h"
#include "util/file-system.h"
#include "util/exceptions/load_exception.h"
#include "util/token.h"
#include "util/tokenreader.h"

using namespace std;
using namespace Platformer;

World::World(const Token * token):
resolutionX(0),
resolutionY(0),
dimensionsX(0),
dimensionsY(0),
gravityX(0),
gravityY(0),
acceleration(0){
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
        TokenReader tr;
        Token * token = tr.readTokenFromFile(filename.path());
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
            } else if (*tok == "mechanics"){
                TokenView mechView = tok->view();
                while (mechView.hasMore()){
                    const Token * mechTok;
                    mechView >> mechTok;
                    if (*mechTok == "gravity"){
                        // get the gravity
                        mechTok->view() >> gravityX >> gravityY;
                    } else if (*mechTok == "acceleration"){
                        // Get the acceleration
                        mechTok->view() >> acceleration;
                    } else {
                        Global::debug( 3 ) << "Unhandled mechanics attribute: "<<endl;
                    }
                }
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
            } else if (*tok == "foreground"){
                Util::ReferenceCount<Background> foreground(new Background(tok, animations));
                foregrounds.push_back(foreground);
            } else if (*tok == "collision-map"){
                collisionMap = Util::ReferenceCount<CollisionMap>(new CollisionMap(tok));
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
    
    // Backgrounds
    for (std::vector< Util::ReferenceCount<Background> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
        Util::ReferenceCount<Background> background = *i;
        if (background != NULL){
            background->act();
        }
    }
    
    // Objects
    for (std::vector< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); ++i){
        Util::ReferenceCount<Object> object = *i;
        
        // make gravity affect the object and append acceleration
        if (gravityX != 0 && object->getVelocityX() == 0){
            object->setVelocityX(gravityX);
        } else if (gravityX != 0){
            if (gravityX > 0){
                object->addVelocity(acceleration, 0);
            } else if (gravityX < 0){
                object->addVelocity(acceleration * -1, 0);
            }
        }
        if (gravityY != 0 && object->getVelocityY() == 0){
            object->setVelocityY(gravityY);
        } else if (gravityY != 0){
            if (gravityY > 0){
                object->addVelocity(0, acceleration);
            } else if (gravityY < 0){
                object->addVelocity(0, acceleration * -1);
            }
        }
        
        if (collisionMap != NULL){
            object->act(collisionMap);
        }
    }
    
    // foregrounds
    for (std::vector< Util::ReferenceCount<Background> >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i){
        Util::ReferenceCount<Background> foreground = *i;
        foreground->act();
    }
    
    scriptEngine->act();
}

void World::draw(const Graphics::Bitmap & bmp){
    // Go through all cameras
    for (std::map< int, Util::ReferenceCount<Camera> >::iterator c = cameras.begin(); c != cameras.end(); ++c){
        Util::ReferenceCount<Camera> camera = c->second;
                
        // Fill to black for now
        camera->getWindow().fill(Graphics::makeColor(0,0,0));
        
        // Backgrounds
        for (std::vector< Util::ReferenceCount<Background> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
            Util::ReferenceCount<Background> background = *i;
            if (background != NULL){
                background->draw(*camera);
            }
        }
        
        // Render objects to camera
        for (std::vector< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); ++i){
            Util::ReferenceCount<Object> object = *i;
            object->draw(*camera);
        }
        
        // Render collision maps (NOTE Debugging only remove later)
        if (collisionMap != NULL){
            collisionMap->render(*camera);
        }
        
        // foregrounds
        for (std::vector< Util::ReferenceCount<Background> >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i){
            Util::ReferenceCount<Background> foreground = *i;
            foreground->draw(*camera);
        }
        
        // Render scriptable items to camera
        scriptEngine->render(*camera);
        
        // Render camera to bmp
        camera->draw(bmp);
    }
}

void World::setCamera(int id, double x, double y){
    std::map< int, Util::ReferenceCount<Camera> >::iterator found = cameras.find(id);
    if (found != cameras.end()){
        found->second->set(x,y);
    }
}

void World::moveCamera(int id, double x, double y){
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

void World::addObject(Util::ReferenceCount<Object> object){
    objects.push_back(object);
}
