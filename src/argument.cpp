#include "argument.h"
#include "game/game.h"
#include "r-tech1/debug.h"
#include "r-tech1/pointer.h"

class PlatformerArgument: public Argument::Parameter {
public:
    std::vector<std::string> keywords() const {
        std::vector<std::string> out;
        out.push_back("platformer");
        out.push_back("--platformer");
        return out;
    }

    std::string description() const {
        return " <game.txt> : Execute platformer mode with given <game.txt> [EXPERIMENTAL]";
    }

    class Run: public Argument::Action {
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

    std::vector<std::string>::iterator parse(std::vector<std::string>::iterator current, std::vector<std::string>::iterator end, Argument::ActionRefs & actions){
        current++;
        if (current != end){
            actions.push_back(::Util::ReferenceCount<Argument::Action>(new Run(*current)));
        } else {
            Global::debug(0) << "Expected an argument. Example: platformer game.txt" << std::endl;
        }
        return current;
    }
};

std::vector< ::Util::ReferenceCount<Argument::Parameter> > Platformer::arguments(){
    std::vector< ::Util::ReferenceCount<Argument::Parameter> > all;
    all.push_back(::Util::ReferenceCount<Argument::Parameter>(new PlatformerArgument()));
    return all;
}
