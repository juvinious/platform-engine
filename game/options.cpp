#include "options.h"
#include "util/menu/menu.h"
#include "util/menu/menu_option.h"
#include "util/menu/options.h"
#include "util/menu/menu-exception.h"
#include "util/token.h"
#include "game.h"

using namespace std;

namespace Platformer{

OptionFactory::OptionFactory(){
}

class OptionPlatformer: public MenuOption {
public:
    OptionPlatformer(const Gui::ContextBox & parent, const Token * token):
    MenuOption(parent, token){
        readName(token);

        if (!token->match("_/game", gameLocation)){
            throw LoadException(__FILE__, __LINE__, "Missing game location for Platformer");
        }
    }

    virtual ~OptionPlatformer(){
    }

    virtual void logic(){
    }

    virtual void run(const Menu::Context & context){
        Game game(gameLocation);
        game.run();
        
        throw Menu::Reload(__FILE__, __LINE__);
    }

protected:
    std::string gameLocation;
};

MenuOption * OptionFactory::getOption(const Gui::ContextBox & parent, const Token *token) const {
    const Token * child;
    token->view() >> child;

    if (*child == "platformer"){
        return new OptionPlatformer(parent, child);
    }

    return NULL;
}

OptionFactory::~OptionFactory(){
}

}
