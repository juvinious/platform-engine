#include "game.h"

#include "platformer/resources/camera.h"
#include "util/gui/cutscene.h"
#include "world.h"
#include "platformer/resources/collisions.h"

#include "util/graphics/bitmap.h"
#include "util/debug.h"
#include "util/events.h"
#include "util/font.h"
#include "util/init.h"
#include "util/input/input-map.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "util/file-system.h"
#include "util/exceptions/load_exception.h"
#include "util/token.h"
#include "util/tokenreader.h"

#include <string>
#include <sstream>

using namespace std;
using namespace Platformer;

Game::Game(const std::string & filename){
    /* NOTE this is temporary to run tests on the engine
     * Eventually it will be replaced with a class that handles the entire game (worlds, etc...)
     */
    try {
        Global::debug(1,"platformer") << "Loading Platformer: " << filename << endl;
        TokenReader tr;
        Token * platformToken = tr.readTokenFromFile(filename);
        
        if ( *platformToken != "platformer" ){
            throw LoadException(__FILE__, __LINE__, "Not a Platformer");
        }

        TokenView view = platformToken->view();
        while (view.hasMore()){
            try{
                const Token * tok;
                view >> tok;
                if ( *tok == "world" ){
                    world = Util::ReferenceCount<Platformer::World>(new Platformer::World(tok));
                } else if ( *tok == "cutscene" ){
                    //std::string file;
                    //tok->view() >> file;
                    //Util::ReferenceCount<Gui::CutScene> cutscene(new Gui::CutScene(tok));
                    //Util::ReferenceCount<Gui::CutScene> cutscene(new Gui::CutScene(Filesystem::AbsolutePath(file)));
                    //cutscenes[cutscene->getName()] = cutscene;
                } else {
                    Global::debug(3) << "Unhandled Platformer attribute: " << endl;
                    if (Global::getDebug() >= 3){
                        tok->print(" ");
                    }
                }
            } catch ( const TokenException & ex ) {
                throw LoadException(__FILE__, __LINE__, ex, "Platformer parse error");
            } catch ( const LoadException & ex ) {
                // delete current;
                throw ex;
            }
        }
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading platformer file.");
    }
    
    // TODO remove test intro cutscene
    /*if (cutscenes["intro"] != NULL){
        cutscenes["intro"]->setResolution(worlds[0]->getResolutionX(), worlds[0]->getResolutionY());
    }*/
}

Game::~Game(){
}

class DrawLogic: public Util::Logic, public Util::Draw {
public:
    
    //! keys
    enum Keys{
        Up,
        Down,
        Left,
        Right,
        Esc,
        Key1,
        Key2,
    };
    
    struct KeyState{
    KeyState():
        esc(false),
        up(false),
        down(false),
        left(false),
        right(false),
        key1(false),
        key2(false){}
        bool esc,up,down,left,right,key1,key2;
    };
    
    DrawLogic(Util::ReferenceCount<Object> & object, Util::ReferenceCount<Platformer::World> & world):
    isDone(false),
    source(true),
    object(object),
    world(world){
        // set input
        input.set(Keyboard::Key_ESC, Esc);
        input.set(Keyboard::Key_UP, Up);
        input.set(Keyboard::Key_DOWN, Down);
        input.set(Keyboard::Key_LEFT, Left);
        input.set(Keyboard::Key_RIGHT, Right);
        input.set(Keyboard::Key_1, Key1);
        input.set(Keyboard::Key_2, Key2);
    }
    bool isDone;
    InputMap<Keys> input;
    InputSource source;
    KeyState keystate;
    Util::ReferenceCount<Object> & object;
    Util::ReferenceCount<Platformer::World> & world;
    
    bool done(){
        return isDone;
    }

    void run(){
        // Keys
        class Handler: public InputHandler<Keys> {
        public:
            Handler(KeyState & keystate):
            keystate(keystate){
            }

            KeyState & keystate;

            void release(const Keys & input, Keyboard::unicode_t unicode){
                switch (input){
                    case Esc:
                        keystate.esc = false;
                        break;
                    case Up: {
                        keystate.up = false;
                        break;
                    }
                    case Down: {
                        keystate.down = false;
                        break;
                    }
                    case Left: {
                        keystate.left = false;
                        break;
                    }
                    case Right: {
                        keystate.right = false;
                        break;
                    }
                    case Key1: {
                        keystate.key1 = true;
                        break;
                    }
                    case Key2: {
                        keystate.key2 = true;
                        break;
                    }
                }
            }

            void press(const Keys & input, Keyboard::unicode_t unicode){
                switch (input){
                    case Esc:
                        keystate.esc = true;
                        break;
                    case Up: {
                        keystate.up = true;
                        break;
                    }
                    case Down: {
                        keystate.down = true;
                        break;
                    }
                    case Left: {
                        keystate.left = true;
                        break;
                    }
                    case Right: {
                        keystate.right = true;
                        break;
                    }
                    case Key1: {
                        keystate.key1 = false;
                        break;
                    }
                    case Key2: {
                        keystate.key2 = false;
                        break;
                    }
                }
            }
        };

        Handler handler(keystate);
        InputManager::handleEvents(input, source, handler);
        
        isDone = keystate.esc;
        
        if (keystate.up){
            if (world->getCamera(0)->isFollowing()){
                if (object->getVelocityY() > -3){
                    object->addVelocity(0,-.2);
                }
            } else {
                world->getCamera(0)->move(0, -5);
            }
        }
        if (keystate.down){
            if (world->getCamera(0)->isFollowing()){
                if (object->getVelocityY() < 3){
                    object->addVelocity(0,.2);
                }
            } else {
                world->getCamera(0)->move(0, 5);
            }
        }
        if (keystate.left){
            if (world->getCamera(0)->isFollowing()){
                if (object->getVelocityX() > -3){
                    object->addVelocity(-.2,0);
                }
            } else {
                world->getCamera(0)->move(-5, 0);
            }
        }
        if (keystate.right){
            if (world->getCamera(0)->isFollowing()){
                if (object->getVelocityX() < 3){
                    object->addVelocity(.2,0);
                }
            } else {
                world->getCamera(0)->move(5, 0);
            }
        }
        if (keystate.key1){
            if (world->getCamera(0)->isFollowing()){
                world->getCamera(0)->stopFollowing();
            } else {
                world->getCamera(0)->followObject(object);
            }
            keystate.key1 = false;
        }
        
        if (keystate.key2){
            world->invokeScript("script", "createObject");
            keystate.key2 = false;
        }
        
        world->act();
    }

    double ticks(double system){
        return Global::ticksPerSecond(60) * system;
    }
    
    void draw(const Graphics::Bitmap & buffer){
        Graphics::StretchedBitmap work(world->getResolutionX(), world->getResolutionY(), buffer);
        work.start();
        work.clear();
        world->draw(work);
        ostringstream info;
        
        info << "FPS: " << getFps();
        Font::getDefaultFont().printf( 10, 10, Graphics::makeColor(255,255,255), work, info.str(), 0);
        info.str(std::string());
        info << "Camera Info - X: " << world->getCamera(0)->getX() << " Y: " << world->getCamera(0)->getY();
        Font::getDefaultFont().printf( 10, 25, Graphics::makeColor(255,255,255), work, info.str(), 0);
        info.str(std::string());
        info << "Object Info - X: " << object->getX() << " Y: " << object->getY();
        Font::getDefaultFont().printf( 10, 40, Graphics::makeColor(255,255,255), work, info.str(), 0);
        info.str(std::string());
        info << "Object Velocity X: " << object->getVelocityX() << " Velocity Y: " << object->getVelocityY();
        Font::getDefaultFont().printf( 10, 55, Graphics::makeColor(255,255,255), work, info.str(), 0);
        work.finish();
    }
};

class TestObject : public Object{
public:
    TestObject():
    hasCollided(false){
        x = 25;
        y = 200;
        label = "player";
    }
    virtual ~TestObject(){}

    void rectDraw(const Collisions::Area & area, double portx, double porty, const Graphics::Bitmap & bmp, bool collision){
        const double viewx = area.x - portx;
        const double viewy = area.y - porty;
        
        bmp.rectangle(viewx, viewy, viewx+area.width, viewy+area.height, 
                                                 (collision ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));
    }
    
    void act(const Util::ReferenceCount<Collisions::Map> collisionMap, std::vector< Util::ReferenceCount<Object> > &){
        
        class Collider : public Collisions::Body{
        public:
            Collider(TestObject & object):
            object(object){
                area.x = object.getX();
                area.y = object.getY();
                area.width = object.getWidth();
                area.height = object.getHeight();
                velocityX = object.getVelocityX();
                velocityY = object.getVelocityY();
            }
            ~Collider(){}
            TestObject & object;
            
            void response(const Collisions::Info & info) const {
                bool collided = false;
                if (info.top){
                    object.setVelocityY(0);
                    collided = true;
                }
                if (info.bottom){
                    object.setVelocityY(0);
                    collided = true;
                }
                if (info.left){
                    object.setVelocityX(0);
                    collided = true;
                }
                if (info.right){
                    object.setVelocityX(0);
                    collided = true;
                }
                object.setCollided(collided);
            }
            
            void noCollision() const {
                object.setCollided(false);
            }
        };
        
        Collider collider(*this);
        collisionMap->collides(collider);
        
        x += velocityX;
        y += velocityY;
    }

    void draw(const Camera & camera){
        if (x >= (camera.getX() - width) && 
            x <= (camera.getX() + camera.getWidth()) &&
            y >= (camera.getY() - height) &&
            y <= (camera.getY() + camera.getHeight())){
                Collisions::Area area(x, y, width, height);
                rectDraw(area, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
        }
    }
    void setCollided(bool collided){
        this->hasCollided = collided;
    }
private:
    bool hasCollided;
};

void Game::run(){
    // NOTE Testing purposes only
    
    /*if (cutscenes["intro"] != NULL){
        cutscenes["intro"]->playAll();
    }*/
    
    // Create dummy object
    Util::ReferenceCount<Object> object = Util::ReferenceCount<Object>(new TestObject());
    world->getCamera(0)->followObject(object);
    world->addObject(object);
    
    DrawLogic logic(object, world);

    Util::standardLoop(logic, logic);
}
