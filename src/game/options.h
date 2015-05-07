#ifndef _platformer_game_menu_options_h
#define _platformer_game_menu_options_h

#include "util/menu/optionfactory.h"

class MenuOption;
namespace Menu{
    class Context;
}

namespace Gui{
    class ContextBox;
}

namespace Platformer{

class OptionFactory: public Menu::OptionFactory {
public:
    OptionFactory();
    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const;
    virtual ~OptionFactory();
};

}

#endif
