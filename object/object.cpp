#include "object.h"

#include "platformer/game/camera.h"

#include "util/debug.h"
#include "util/token.h"
#include "util/graphics/bitmap.h"

using namespace std;
using namespace Platformer;

Object::Object():
followed(false),
attachedCamera(0),
x(0),
y(0),
width(15),
height(15),
life(0),
invincible(false),
collided(false){
    
}

Object::~Object(){
    
}

void Object::act(){
}

void Object::draw(const Camera & camera){
    if (x >= camera.getX() && 
        x <= (camera.getX() + camera.getWidth()) &&
        y >= camera.getY() &&
        y <= (camera.getY() + camera.getHeight())){
            
            const int viewx = (x > camera.getX() ? x - camera.getX() : camera.getX() - x);
            const int viewy = (y > camera.getY() ? y - camera.getY() : camera.getY() - y);
            camera.getWindow().rectangleFill(viewx, viewy, viewx+width, viewy+height, 
                                             (collided ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));
    }
}

void Object::set(int x, int y){
    this->x = x;
    this->y = y;
}

void Object::move(int x, int y){
    this->x+=x;
    this->y+=y;
}
