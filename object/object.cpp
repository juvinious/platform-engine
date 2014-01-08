#include "object.h"

#include "platformer/game/camera.h"
#include "platformer/game/collision-map.h"

#include "util/debug.h"
#include "util/token.h"
#include "util/graphics/bitmap.h"

using namespace Platformer;

static int OBJECT_IDS = 0;

Object::Object():
id(OBJECT_IDS++),
followed(false),
attachedCamera(0),
x(0),
y(0),
width(20),
height(20),
velocityX(0),
velocityY(0){
    
}

Object::~Object(){
    
}

void Object::act(const Util::ReferenceCount<CollisionMap>, std::vector< Util::ReferenceCount<Object> > &){
}

void Object::draw(const Camera &){
}

void Object::set(double x, double y){
    this->x = x;
    this->y = y;
}

void Object::move(double x, double y){
    this->x+=x;
    this->y+=y;
}
