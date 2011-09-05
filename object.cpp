#include "object.h"

#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token.h"


using namespace std;
using namespace Platformer;

Object::Object():
followed(false),
attachedCamera(0),
x(0),
y(0),
width(0),
height(0),
life(0),
invincible(false){
    
}

Object::~Object(){
    
}

void Object::act(){
}
