#include "game.h"

#include "camera.h"
#include "util/gui/cutscene.h"
#include "world.h"

#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/debug.h"
#include "util/events.h"
#include "util/font.h"
#include "util/init.h"
#include "util/input/input-map.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "util/file-system.h"
#include "util/load_exception.h"
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
	TokenReader tr(Filesystem::AbsolutePath(filename).path());
    
        Token * platformToken = tr.readToken();
    
    
	if ( *platformToken != "platformer" ){
	    throw LoadException(__FILE__, __LINE__, "Not a Platformer");
	}

	TokenView view = platformToken->view();
	while (view.hasMore()){
	    try{
		const Token * tok;
		view >> tok;
		if ( *tok == "world" ){
                    worlds.push_back(new Platformer::World(tok));
                } else if ( *tok == "cutscene" ){
                    Util::ReferenceCount<Gui::CutScene> cutscene = new Gui::CutScene(tok);
                    cutscenes[cutscene->getName()] = cutscene;
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
    cutscenes["intro"]->setResolution(worlds[0]->getResolutionX(), worlds[0]->getResolutionY());
}

Game::~Game(){
}

void Game::run(){
    // NOTE Testing purposes only
    
    class Logic: public Util::Logic {
    public:
        Logic(InputMap<Keys> & input, std::vector < Util::ReferenceCount<Platformer::World> > & worlds):
        is_done(false),
        input(input),
        worlds(worlds){
        }

        bool is_done;
	InputMap<Keys> & input;
	std::vector < Util::ReferenceCount<Platformer::World> > & worlds;
        
	bool done(){
            return is_done;
        }

	void run(){
	    // FIXME figure out how many worlds... etc
            vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource());
	    for (vector<InputMap<Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc){
                    is_done = true;
                }
                if (event.out == Up){
                    worlds[0]->moveCamera(0, 0,-5);
                }
                if (event.out == Down){
                    worlds[0]->moveCamera(0, 0,5);
                }
                if (event.out == Left){
                    worlds[0]->moveCamera(0, -5,0);
                }
                if (event.out == Right){
                    worlds[0]->moveCamera(0, 5,0);
                }
                if (event.out == K_1){
                    worlds[0]->moveCamera(1, -5,0);
                }
                if (event.out == K_2){
                    worlds[0]->moveCamera(1, 5,0);
                }
                if (event.out == K_3){
                    worlds[0]->moveCamera(1, 0, -5);
                }
                if (event.out == K_4){
                    worlds[0]->moveCamera(1, 0, 5);
                }
            }
        }
        
        worlds[0]->act();
        
        }

        double ticks(double system){
            return system * Global::LOGIC_MULTIPLIER;
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(std::vector < Util::ReferenceCount<Platformer::World> > & worlds, const Logic & logic):
        worlds(worlds),
        logic(logic){
        }
        
        std::vector < Util::ReferenceCount<Platformer::World> > & worlds;

	const Logic & logic;

        void draw(const Graphics::Bitmap & buffer){
            // FIXME change this later as the actual resolution is in the world configuration
            Graphics::StretchedBitmap work(worlds[0]->getResolutionX(), worlds[0]->getResolutionY(), buffer);
            work.start();
            worlds[0]->draw(work);
            ostringstream info;
            info << "Camera Info - X: " << worlds[0]->getCamera(0)->getX() << " Y: " << worlds[0]->getCamera(0)->getY();
            Font::getDefaultFont().printf( 10, 10, Graphics::makeColor(255,255,255), work, info.str(), 0);
            info.str("");
            info << "Camera Info - X: " << worlds[0]->getCamera(1)->getX() << " Y: " << worlds[0]->getCamera(1)->getY();
            Font::getDefaultFont().printf( 10, 30, Graphics::makeColor(255,255,255), work, info.str(), 0);
            work.finish();
            buffer.BlitToScreen();
        }
    };
    
    // set input
    input.set(Keyboard::Key_ESC, 0, true, Esc);
    input.set(Keyboard::Key_UP, 0, true, Up);
    input.set(Keyboard::Key_DOWN, 0, true, Down);
    input.set(Keyboard::Key_LEFT, 0, true, Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Right);
    input.set(Keyboard::Key_1, 0, true, K_1);
    input.set(Keyboard::Key_2, 0, true, K_2);
    input.set(Keyboard::Key_3, 0, true, K_3);
    input.set(Keyboard::Key_4, 0, true, K_4);
    
    // Graphics::Bitmap tmp(640, 480);
    
    while (cutscenes["intro"]->hasMore()){
        cutscenes["intro"]->next();
    }

    Logic logic(input, worlds);
    Draw draw(worlds, logic);

    Util::standardLoop(logic, draw);
}
