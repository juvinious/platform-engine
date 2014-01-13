#include "world.h"

#include "platformer/resources/animation.h"
#include "platformer/resources/background.h"
#include "platformer/resources/camera.h"
#include "platformer/resources/collisions.h"
#include "platformer/resources/control.h"
#include "platformer/script/script.h"

#include "util/debug.h"
#include "util/file-system.h"
#include "util/exceptions/load_exception.h"
#include "util/exceptions/exception.h"
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
acceleration(0),
fillColor(Graphics::makeColor(0,0,0)),
quitRequest(false),
paused(false){
    if ( *token != "world" ){
        throw LoadException(__FILE__, __LINE__, "Not world.");
    }
    
    //! Setup script engine
    scriptEngine = Scriptable::getInstance(this);
    
    if (token->numTokens() == 1){
        std::string temp;
        token->view() >> temp;
        load(Storage::instance().find(Filesystem::RelativePath(temp)));
    } else {
        load(token);
    }
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
            } else if (*tok == "fill-color"){
                int r, g, b;
                tok->view() >> r >> g >> b;
                fillColor = Graphics::makeColor(r,g,b);
            } else if (*tok == "camera"){
                // Handle camera info
                Util::ReferenceCount<Camera> camera = Util::ReferenceCount<Camera>(new Camera(resolutionX, resolutionY, dimensionsX, dimensionsY, tok));
                CameraInfo info = { -1 , camera };
                cameras[camera->getId()] = info;
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
                collisionMap = Util::ReferenceCount<Collisions::Map>(new Collisions::Map(tok));
            } else if (*tok == "script"){
                std::string module, function;
                TokenView scriptView = tok->view();
                while (scriptView.hasMore()){
                    const Token * scriptTok;
                    scriptView >> scriptTok;
                    if (*scriptTok == "id"){
                        // get the name?
                    } else if (*scriptTok == "module"){
                        // Get the module
                        scriptTok->view() >> module;
                    } else if (*scriptTok == "function"){
                        // Get the function
                        scriptTok->view() >> function;
                    } else {
                        Global::debug( 3 ) << "Unhandled script attribute: "<<endl;
                    }
                }
                if (!module.empty() && !function.empty()){
                    scriptEngine->runScript(module, function);
                }
            } else if (*tok == "object-script"){
                scriptEngine->importObject(tok);
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
    if (!paused){
        for (std::map< int, CameraInfo >::iterator c = cameras.begin(); c != cameras.end(); ++c){
            Util::ReferenceCount<Camera> camera = c->second.camera;
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
        for (std::deque< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); ++i){
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
                object->act(collisionMap, objects);
            }
        }
        
        // foregrounds
        for (std::vector< Util::ReferenceCount<Background> >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i){
            Util::ReferenceCount<Background> foreground = *i;
            foreground->act();
        }
    }
    
    // Controls
    for (std::map< int, Util::ReferenceCount<Control> >::iterator i = controls.begin(); i != controls.end(); i++){
        Util::ReferenceCount<Control> control = i->second;
        control->act();
    }
    
    scriptEngine->act(paused);
    
    if (quitRequest){
        throw Exception::Quit(__FILE__, __LINE__);
    }
}

void World::draw(const Graphics::Bitmap & bmp){
    // Go through all cameras
    for (std::map< int, CameraInfo >::iterator c = cameras.begin(); c != cameras.end(); ++c){
        Util::ReferenceCount<Camera> camera = c->second.camera;
                
        // Fill to color
        camera->getWindow().fill(fillColor);
        
        // Backgrounds
        for (std::vector< Util::ReferenceCount<Background> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
            Util::ReferenceCount<Background> background = *i;
            if (background != NULL){
                background->draw(*camera);
            }
        }
        
        // Render objects to camera
        for (std::deque< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); ++i){
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
    std::map<int, CameraInfo>::iterator found = cameras.find(id);
    if (found != cameras.end()){
        found->second.camera->set(x,y);
    }
}

void World::moveCamera(int id, double x, double y){
    std::map<int, CameraInfo>::iterator found = cameras.find(id);
    if (found != cameras.end()){
        found->second.camera->move(x,y);
    }
}

void World::followNextObject(int id){
    std::map<int, CameraInfo>::iterator found = cameras.find(id);
    if (found != cameras.end()){
        if (found->second.currentObject >= -1 && found->second.currentObject < (int)objects.size()-1){
            found->second.currentObject++;
            found->second.camera->stopFollowing();
            Util::ReferenceCount<Object> object = objects[(unsigned int)found->second.currentObject];
            found->second.camera->followObject(object);
        } else {
            found->second.currentObject = -1;
            found->second.camera->stopFollowing();
        }
    }
}

void World::followObject(int cameraId, int objectId){
    for (unsigned int i = 0; i < objects.size(); i++){
        Util::ReferenceCount<Object> object = objects[i];
        if (object->getID() == objectId){
            std::map<int, CameraInfo>::iterator found = cameras.find(cameraId);
            if (found != cameras.end()){
                found->second.camera->stopFollowing();
                found->second.currentObject = i;
                found->second.camera->followObject(object);
            }
            break;
        }
    }
}

Util::ReferenceCount<Camera> World::getCamera(int id){
    std::map<int, CameraInfo>::iterator found = cameras.find(id);
    if (found != cameras.end()){
        return found->second.camera;
    }
    
    return Util::ReferenceCount<Camera>(NULL);
}

void World::addObject(Util::ReferenceCount<Object> object){
    objects.push_front(object);
}

Util::ReferenceCount<Object> World::getObject(int id){
    for (std::deque< Util::ReferenceCount<Object> >::iterator i = objects.begin(); i != objects.end(); i++){
        Util::ReferenceCount<Object> object = *i;
        if (object->getID() == id){
            return object;
        }
    }
    
    return Util::ReferenceCount<Object>(NULL);
}

void World::addControl(Util::ReferenceCount<Control> control){
    controls.insert(std::pair<int, Util::ReferenceCount<Control> >(control->getID(), control));
}

Util::ReferenceCount<Control> World::getControl(int id){
    std::map<int, Util::ReferenceCount<Control> >::iterator found = controls.find(id);
    if (found != controls.end()){
        return found->second;
    }
    
    return Util::ReferenceCount<Control>(NULL);
}

void World::invokeScript(const std::string & module, const std::string & func){
    scriptEngine->runScript(module, func);
}
