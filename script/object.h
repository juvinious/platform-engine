#ifndef platformer_script_object_h
#define platformer_script_object_h

#ifdef HAVE_PYTHON

#include "util/pointer.h"
#include "platformer/object/object.h"
#include "platformer/game/animation.h"
#include "python.h"

#include <string>
#include <map>

namespace Graphics{
class Bitmap;
}

namespace Util{
class Token;
}

namespace Platformer{
    
class Camera;
class Object;
class World;
class Animation;

class ScriptObject : public Object{
public:
    ScriptObject(const std::string &, const std::string &);
    virtual ~ScriptObject();
    
    void act(const Util::ReferenceCount<Platformer::Collisions::Map>, std::vector< Util::ReferenceCount<Object> > &);
    void draw(const Platformer::Camera &);
    
    void add(const std::string &, const Script::Runnable &);
    
    void addAnimation(const Token *);
    
    void setCurrentAnimation(const std::string &);
    
    static PyMethodDef * Methods;
private:
    // Animations
    typedef std::map<std::string, Util::ReferenceCount<Animation> > AnimationMap;
    AnimationMap::iterator currentAnimation;
    AnimationMap animations;
    
    // Scripts
    Script::RunMap scripts;
};

}
#endif
#endif
