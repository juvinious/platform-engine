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
width(20),
height(20),
life(0),
invincible(false),
hasCollided(false){
    
}

Object::~Object(){
    
}

void Object::act(){
    // Update areas
    main.x = left.x = x;
    main.y = top.y = y;
    main.width = width;
    main.height = height;
    
    top.x = bottom.x = x + (width * .15);
    top.width = bottom.width = width - (width * .20);
    
    bottom.y = y + height - (height * .15);
    top.height = bottom.height = (height * .15);
    
    right.x = x + width - (width * .20);
    left.width = right.width = (width * .20);
    left.y = right.y = y + (height * .15);
    left.height = right.height = height - (height * .20);
}

void Object::collided(const CollisionInfo & info){
    switch (info.type){
        case CollisionInfo::Top:
            Global::debug(3) << "Hit top!" << std::endl;
            y = info.area.y - height;
            break;
        case CollisionInfo::Bottom:
            Global::debug(3) << "Hit bottom!" << std::endl;
            y = info.area.y + info.area.height;
            break;
        case CollisionInfo::Left:
            Global::debug(3) << "Hit left!" << std::endl;
            x = info.area.x - width;
            break;
        case CollisionInfo::Right:
            Global::debug(3) << "Hit right!" << std::endl;
            x = info.area.x + info.area.width;
            break;
        default:
            break;
    }
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
            
            /*const int viewx = (x > camera.getX() ? x - camera.getX() : camera.getX() - x);
            const int viewy = (y > camera.getY() ? y - camera.getY() : camera.getY() - y);
            camera.getWindow().rectangleFill(viewx, viewy, viewx+width, viewy+height, 
                                             (hasCollided ? Graphics::makeColor(255, 0, 0) : Graphics::makeColor(128,128,128)));*/
            //rectDraw(main, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
            rectDraw(top, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
            rectDraw(bottom, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
            rectDraw(right, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
            rectDraw(left, camera.getX(), camera.getY(), camera.getWindow(), hasCollided);
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
