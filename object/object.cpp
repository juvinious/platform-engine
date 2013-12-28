#include "object.h"

#include "platformer/game/camera.h"
#include "platformer/game/collision-map.h"

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
width(20),
height(20),
velocityX(0),
velocityY(0),
life(0),
invincible(false),
hasCollided(false){
    
}

Object::~Object(){
    
}

void Object::act(){
    x += velocityX;
    y += velocityY;
}

static void rectDraw(Area area, int portx, int porty, const Graphics::Bitmap & bmp, bool collision){
    const int viewx = (area.x > portx ? area.x - portx : portx - area.x);
    const int viewy = (area.y > porty ? area.y - porty : porty - area.y);
    bmp.rectangle(viewx, viewy, viewx+area.width, viewy+area.height, 
                                             (collision ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));
}

void Object::draw(const Camera & camera){
    if (x >= camera.getX() && 
        x <= (camera.getX() + camera.getWidth()) &&
        y >= camera.getY() &&
        y <= (camera.getY() + camera.getHeight())){
            Area area = {x, y, width, height};
            rectDraw(area, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
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
