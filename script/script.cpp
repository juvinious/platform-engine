#include "script.h"

#include "platformer/game/camera.h"
#include "platformer/game/world.h"
#include "platformer/object/object.h"

using namespace Platformer;

Scriptable::Scriptable(){
}

Scriptable::~Scriptable(){
}

Util::ReferenceCount<Scriptable> Scriptable::getScriptable(){
#ifdef HAVE_PYTHON
    return Util::ReferenceCount<Scriptable>(new NoScript());
#else
    return Util::ReferenceCount<Scriptable>(new NoScript());
#endif
}


NoScript::NoScript(){
}
NoScript::~NoScript(){
}
void NoScript::init(World *){
}
void NoScript::act(){
}
void NoScript::render(const Camera &){
}
void NoScript::registerAnimation(void *){
}
void NoScript::registerObject(void *){
}
