#include "collision-map.h"

#include "platformer/object/object.h"
#include "platformer/game/camera.h"

#include "util/debug.h"
#include "util/token.h"
#include "util/graphics/bitmap.h"
#include "util/exceptions/load_exception.h"

#include <math.h>

namespace Platformer{
    
static bool within(const Area & area1, const Area & area2){
    return !((area1.x > area2.x + area2.width - 1) ||
        (area1.y > area2.y + area2.height - 1) ||
        (area2.x > area1.x + area1.width - 1) ||
        (area2.y > area1.y + area1.height - 1));
}

CollisionBody::CollisionBody():
velocityX(0),
velocityY(0){
}

CollisionBody::~CollisionBody(){
}

CollisionMap::CollisionMap(const Token * token){
    if (*token != "collision-map"){
        throw LoadException(__FILE__, __LINE__, "Not a collision map.");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "area"){
                // get the name
                Area area;
                tok->view() >> area.x >> area.y >> area.width >> area.height;
                regions.push_back(area);
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

CollisionMap::~CollisionMap(){
}

void CollisionMap::collides(const CollisionBody & body){
    Area nextMovement = body.getArea();
    // Scan ahead
    nextMovement.x += body.getVelocityX();
    nextMovement.y += body.getVelocityY();
    bool collides = false;
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        if (within(nextMovement, area)){
            CollisionInfo info;
            info.top = info.bottom = info.left = info.right = false;
            // FIXME need to figure out where the collision occurs
            if (nextMovement.getX2() >= area.x && body.getArea().getX2() <= area.x){
                // Collision is happening at the top
                info.left = true;
            }
            if (nextMovement.getY2() >= area.y && body.getArea().getY2() <= area.y){
                // Collision is happening at the left
                info.top = true;
            }
            if (nextMovement.x <= area.getX2() && body.getArea().x >= area.getX2()){
                // Collision is happening at the bottom
                info.right = true;
            }
            if (nextMovement.getY2() >= area.y && body.getArea().y >= area.getY2()){
                // Collision is happening at the right
                info.bottom = true;
            }
            body.response(info);
            collides = true;
        }
    }
    if (!collides){
        body.noCollision();
    }
}

void CollisionMap::act(){
    // Check if they collide with bodies
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        // Nothing for now
    }
}

void CollisionMap::render(const Camera & camera){
    Area cameraWindow = { camera.getX(), camera.getY(), camera.getWidth(), camera.getHeight() };
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        if (within(area, cameraWindow)){
            const int x1 = (area.x <= camera.getX() ? 0 : area.x - camera.getX());
            const int y1 = (area.y <= camera.getY() ? 0 : area.y - camera.getY());
            const int x2 = ((area.x + area.width) >= (camera.getX() + camera.getWidth()) ? 
                            0 : (camera.getX() + camera.getWidth()) - (area.x + area.width));
            const int y2 = ((area.y + area.height) >= (camera.getY() + camera.getHeight()) ? 
                            0 : (camera.getY() + camera.getHeight()) - (area.y + area.height));
            // Draw to current screen
            camera.getWindow().rectangle(x1, y1, (camera.getWidth() - x2), (camera.getHeight() - y2), Graphics::makeColor(0,255,0));
        }
    }
}

}
