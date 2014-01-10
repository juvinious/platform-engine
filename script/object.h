#ifndef platformer_script_object_h
#define platformer_script_object_h

#ifdef HAVE_PYTHON

#include "util/pointer.h"
#include "platformer/resources/object.h"
#include "platformer/resources/value.h"
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
    ScriptObject(const Platformer::Script::Runnable &);
    virtual ~ScriptObject();
    
    void act(const Util::ReferenceCount<Platformer::Collisions::Map>, std::vector< Util::ReferenceCount<Object> > &);
    void draw(const Platformer::Camera &);
    
    void add(const std::string &, const Script::Runnable &);
    
    void addAnimation(const Token *);
    
    void setCurrentAnimation(const std::string &);
    
    const std::string getCurrentAnimation() const;
    
    void setValue(const std::string &, const Value &);
    
    const Value getValue(const std::string &);
    
    inline bool getAnimationVerticalFlip() const {
        return this->animationVerticalFlip;
    }
    
    inline bool getAnimationHorizontalFlip() const {
        return this->animationHorizontalFlip;
    }
    
    inline void setAnimationVerticalFlip(bool flip) {
        this->animationVerticalFlip = flip;
    }
    
    inline void setAnimationHorizontalFlip(bool flip) {
        this->animationHorizontalFlip = flip;
    }
    
    static PyMethodDef * Methods;
private:
    // Animations
    typedef std::map<std::string, Util::ReferenceCount<Animation> > AnimationMap;
    AnimationMap::iterator currentAnimation;
    bool animationHorizontalFlip;
    bool animationVerticalFlip;
    AnimationMap animations;
    
    // Scripts
    Script::RunMap scripts;
    
    // Values
    typedef std::map<std::string, Value> ValueMap;
    ValueMap values;
};

}
#endif
#endif
