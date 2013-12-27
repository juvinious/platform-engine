#include "collision-map.h"

#include "platformer/object/object.h"
#include "platformer/game/camera.h"

#include "util/debug.h"
#include "util/token.h"
#include "util/graphics/bitmap.h"
#include "util/exceptions/load_exception.h"

namespace Platformer{
    
static bool within(const Area & area1, const Area & area2){
    return !((area1.x > area2.x + area2.width - 1) ||
        (area1.y > area2.y + area2.height - 1) ||
        (area2.x > area1.x + area1.width - 1) ||
        (area2.y > area1.y + area1.height - 1));
}

static CollisionInfo::CollisionType where(const Area & area1, const Area & area2){
    int leftdepth = 0;
    int rightdepth = 0;
    int topdepth = 0;
    int bottomdepth = 0;
    if ((area1.y + area1.height) > area2.y && (area1.y + area1.height) < (area2.y + area2.height)){
        topdepth = (area1.y + area1.height) - area2.y;
    }
    if ((area1.x + area1.width) > area2.x && (area1.x + area1.width) < (area2.x + area2.width)){
        leftdepth = (area1.x + area1.width) - area2.x;
    }
    if (area1.y < (area2.y + area2.height) && area1.y > area2.y){
        bottomdepth = (area2.y + area2.height) - area1.y;
    }
    if (area1.x < (area2.x + area2.width) && area1.x > area2.x){
        rightdepth = (area2.x + area2.width) - area1.x;
    }
    
    /*if ((topdepth > leftdepth) && (topdepth > rightdepth)){
        return CollisionInfo::Top;
    } else if ((topdepth < leftdepth) && (topdepth > rightdepth)){
        return CollisionInfo::Left;
    }*/
    
    /*if (area2.y < area1.y + area1.height - 1){
        return CollisionInfo::Top;
    } else if (area2.x < area1.x + area1.width - 1){
        return CollisionInfo::Left;
    } else if (area1.x < area2.x + area2.width - 1){
        return CollisionInfo::Right;
    } else if (area1.y < area2.y + area2.height - 1){
        return CollisionInfo::Bottom;
    }*/
    
    return CollisionInfo::Top;
    //return CollisionInfo::None;
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

CollisionInfo CollisionMap::collides(Util::ReferenceCount<Object> object){
    Area objectArea = { object->getX(), object->getY(), object->getWidth(), object->getHeight() };
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        if (within(objectArea, area)){
            CollisionInfo info = { where(objectArea, area), area }; 
            return info;
        }
    }
    CollisionInfo info = { CollisionInfo::None, Area() };
    return info;
}

void CollisionMap::render(const Camera & camera){
    Area cameraWindow = { camera.getX(), camera.getY(), camera.getWidth(), camera.getHeight() };
    for (std::vector<Area>::iterator i = regions.begin(); i != regions.end(); i++){
        const Area & area = *i;
        if (within(area, cameraWindow)){
            /*const int viewx = (area.x > camera.getX() ? area.x - camera.getX() : camera.getX() - area.x);
            const int viewy = (area.y > camera.getY() ? area.y - camera.getY() : camera.getY() - area.y);
            camera.getWindow().rectangle(viewx, viewy, viewx + area.width, viewy + area.height, Graphics::makeColor(0,255,0));*/
            const int x1 = (area.x <= camera.getX() ? 0 : area.x - camera.getX());
            const int y1 = (area.y <= camera.getY() ? 0 : area.y - camera.getY());
            const int x2 = ((area.x + area.width) >= (camera.getX() + camera.getWidth()) ? 
                            0 : (camera.getX() + camera.getWidth()) - (area.x + area.width));
            const int y2 = ((area.y + area.height) >= (camera.getY() + camera.getHeight()) ? 
                            0 : (camera.getY() + camera.getHeight()) - (area.y + area.height));
            //Global::debug(0) << "x ["<< x1 << "] y [" << y1 << "] width [" << x2 << "] height [" << y2 << "]" << std::endl;
            camera.getWindow().rectangle(x1, y1, (camera.getWidth() - x2), (camera.getHeight() - y2), Graphics::makeColor(0,255,0));
        }
    }
}

}