#include "r-tech1/init.h"
#include "r-tech1/debug.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/network/network.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/system.h"
#include "r-tech1/sound/music.h"
#include "r-tech1/menu/menu.h"
#include "r-tech1/menu/menu-exception.h"
#include "r-tech1/menu/optionfactory.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/exceptions/shutdown_exception.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/funcs.h"
#include "r-tech1/ftalleg.h"
#include "r-tech1/file-system.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/token.h"
#include "r-tech1/parameter.h"
#include "r-tech1/version.h"
#include "r-tech1/debug.h"
#include "r-tech1/configuration.h"
#include "r-tech1/init.h"
#include "r-tech1/main.h"
#include "r-tech1/argument.h"
#include "argument.h"
#include "game/game.h"


using namespace std;

static const int DEFAULT_DEBUG = 0;

Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.rtech-platformerrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/rtech-platformer";
    } else {
        str << home << "/.rtech-platformer/";
    }
    return Filesystem::AbsolutePath(str.str());
}


class DefaultGame: public Argument::Action {
public:
    void act(){
        Platformer::Game game("data/platformer/smb/mario.txt");
        game.run();
    }
};

int main(int argc, char ** argv){
    
    /* -1 means use whatever is in the configuration */
    Global::InitConditions conditions;

    bool music_on = true;
    bool allow_quit = true;
    //Collector janitor;

    Version::setVersion(0, 0, 1);

    System::startMemoryUsage();

    Global::setDebug(DEFAULT_DEBUG);
    Global::setDefaultDebugContext("platformer");
    
    Argument::Handler handler;
    
    handler.add(Platformer::arguments());
    handler.setDefaultAction(Util::ReferenceCount<Argument::Action>(new DefaultGame()));
    
    Global::debug(0) << "Debug level: " << Global::getDebug() << std::endl;
    
    Global::debug(0) << "Platformer version " << Version::getVersionString() << std::endl;

    if (! Global::init(conditions)){
        Global::debug(0) << "Could not initialize system" << endl;
        return -1;
    }

    if (! Global::dataCheck()){
        return -1;
    }

    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());
    
    InputManager input;
    Music music(music_on);

    Util::Parameter<Util::ReferenceCount<Graphics::ShaderManager> > defaultShaderManager(Graphics::shaderManager, Util::ReferenceCount<Graphics::ShaderManager>(new Graphics::ShaderManager()));

    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultMenuFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(*defaultFont.current(), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

    try {
        handler.runActions(argc, argv);
    } catch (...){
        Global::debug(0) << "Uncaught exception!" << std::endl;
    }

    //Configuration::saveConfiguration();

    Global::debug(0) << "Bye!" << endl;
    Global::closeLog();
    return 0;
}
