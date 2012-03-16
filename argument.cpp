#include "argument.h"
#include "game/game.h"
#include "util/pointer.h"

class PlatformerArgument: public Argument {
public:
    std::vector<std::string> keywords() const {
        std::vector<std::string> out;
        out.push_back("platform");
        out.push_back("platformer");
        out.push_back("--platform");
        out.push_back("--platformer");
        return out;
    }

    std::string description() const {
        return " <game.txt> : Execute platformer mode with given <game.txt> [EXPERIMENTAL]";
    }

    class Run: public ArgumentAction {
    public:
        Run(const std::string & script):
        script(script){
        }
        virtual void act(){
            Platformer::Game game(script);
            game.run();
        }
        std::string script;
    };

    std::vector<std::string>::iterator parse(std::vector<std::string>::iterator current, std::vector<std::string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(*current)));
        }
        return current;
    }
};

std::vector< ::Util::ReferenceCount<Argument> > Platformer::arguments(){
    std::vector< ::Util::ReferenceCount<Argument> > all;
    all.push_back(::Util::ReferenceCount<Argument>(new PlatformerArgument()));
    return all;
}
