#include "collisions.h"

#include "object.h"
#include "camera.h"

#include "util/debug.h"
#include "util/token.h"
#include "util/graphics/bitmap.h"
#include "util/exceptions/load_exception.h"

#include <math.h>

namespace Platformer{
namespace Collisions{
    
static bool within(const Area & area1, const Area & area2){
    return !((area1.x > area2.getX2()) ||
        (area1.y > area2.getY2()) ||
        (area2.x > area1.getX2()) ||
        (area2.y > area1.getY2()));
}

Area::Area():
x(0),
y(0),
width(0),
height(0){
}

Area::Area(double x, double y, int width, int height):
x(x),
y(y),
width(width),
height(height){
}

Area::Area(const Token * token):
x(0),
y(0),
width(0),
height(0){
    if (*token != "area"){
        throw LoadException(__FILE__, __LINE__, "Not a collision area.");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        const Token * tok;
        view >> tok;
        if (*tok == "position"){
            tok->view() >> x >> y >> width >> height;
        } else {
            Global::debug( 3 ) << "Unhandled Area attribute: " << tok->getName() << std::endl;
        }
    }
}

Area::Area(const Area & copy):
x(copy.x),
y(copy.y),
width(copy.width),
height(copy.height){
}

Area::~Area(){
}

const Area & Area::operator=(const Area & copy) {
    x = copy.x;
    y = copy.y;
    width = copy.width;
    height = copy.height;
    return *this;
}

Body::Body():
velocityX(0),
velocityY(0){
}

Body::~Body(){
}

void Body::noCollision() const {
}

bool Body::collides(const Area & check) const {
    Area nextMovement = area;
    // Scan ahead
    nextMovement.x += velocityX;
    nextMovement.y += velocityY;
    if (within(nextMovement, check)){
        Collisions::Info info;
        info.area = check;
        info.top = info.bottom = info.left = info.right = false;
        // FIXME need to figure out where the collision occurs
        if (nextMovement.getX2() >= check.x && area.getX2() <= check.x){
            // Collision is happening at the top
            info.left = true;
        }
        if (nextMovement.getY2() >= check.y && area.getY2() <= check.y){
            // Collision is happening at the left
            info.top = true;
        }
        if (nextMovement.x <= check.getX2() && area.x >= check.getX2()){
            // Collision is happening at the bottom
            info.right = true;
        }
        if (nextMovement.y <= check.getY2() && area.y >= check.getY2()){
            // Collision is happening at the right
            info.bottom = true;
        }
        response(info);
        return true;
    }
    return false;
}

Map::Map(const Token * token){
    if (*token != "collision-map"){
        throw LoadException(__FILE__, __LINE__, "Not a collision map.");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "area"){
                regions.push_back(Area(tok));
            } else {
                Global::debug( 3 ) << "Unhandled Collision attribute: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Collision parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

Map::~Map(){
}

void Map::collides(const Collisions::Body & body) const{
    bool collides = false;
    for (std::vector<Area>::const_iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        if (body.collides(area)){
            collides = true;
        }
    }
    if (!collides){
        body.noCollision();
    }
}

void Map::act(){
    // Check if they collide with bodies
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        // Nothing for now
    }
}

void Map::render(const Camera & camera){
    Area cameraWindow(camera.getX(), camera.getY(), camera.getWidth(), camera.getHeight());
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        if (within(area, cameraWindow)){
            const double x1 = (area.x <= camera.getX() ? 0 : area.x - camera.getX());
            const double y1 = (area.y <= camera.getY() ? 0 : area.y - camera.getY());
            const double x2 = ((area.x + area.width) >= (camera.getX() + camera.getWidth()) ? 
                            0 : (camera.getX() + camera.getWidth()) - (area.x + area.width));
            const double y2 = ((area.y + area.height) >= (camera.getY() + camera.getHeight()) ? 
                            0 : (camera.getY() + camera.getHeight()) - (area.y + area.height));
            // Draw to current screen
            camera.getWindow().rectangle(x1, y1, (camera.getWidth() - x2), (camera.getHeight() - y2), Graphics::makeColor(0,255,0));
        }
    }
}

}
}
