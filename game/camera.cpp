#include "camera.h"

#include "util/graphics/bitmap.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token.h"


using namespace std;
using namespace Platformer;

Camera::Camera(int resolutionX, int resolutionY, int worldWidth, int worldHeight, const Token * token):
id(0),
resolutionX(resolutionX),
resolutionY(resolutionY),
worldWidth(worldWidth),
worldHeight(worldHeight),
x(0),
y(0),
currentX(0),
currentY(0),
windowX(0),
windowY(0),
windowWidth(0),
windowHeight(0),
window(NULL),
scrollSpeed(2),
currentXSpeed(0),
currentYSpeed(0),
velocity(.5),
smoothScrolling(false),
smoothScrollWaitX(5),
smoothScrollWaitY(5),
smoothScrollModifier(5),
follow(false),
followVariance(1.5),
object(NULL){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "id"){
                // get the name
                tok->view() >> id;
            } else if (*tok == "dimensions"){
                // Get the resolution of the world
                tok->view() >> worldWidth >> worldHeight;
            } else if (*tok == "start"){
                // Starting point for camera
                tok->view() >> x >> y;
            } else if (*tok == "viewport"){
                // The viewing area of the camera
                tok->view() >> windowX >> windowY >> windowWidth >> windowHeight;
            } else if (*tok == "speed"){
                // Scroll speed
                tok->view() >> scrollSpeed;
            } else if (*tok == "velocity"){
                // Scroll speed
                tok->view() >> velocity;
            } else if (*tok == "follow-variance"){
                // Follow variance of the camera when following an object
                tok->view() >> followVariance;
            } else if (*tok == "smooth-scrolling"){
                // Follow variance of the camera when following an object
                tok->view() >> smoothScrolling;
            } else if (*tok == "smooth-scroll-modifier"){
                // Follow variance of the camera when following an object
                tok->view() >> smoothScrollModifier;
            } else {
                Global::debug( 3 ) << "Unhandled Camera attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Camera parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
    
    // Set up window
    window = new Graphics::Bitmap(getViewportWidth(), getViewportHeight());
}

Camera::~Camera(){
}

void Camera::set(int x, int y){
    this->x = x;
    this->y = y;
    
    checkBounds();
}

void Camera::move(int x, int y){
    this->x+=x;
    this->y+=y;
    
    checkBounds();
}

void Camera::followObject(Util::ReferenceCount<Object> obj){
    object = obj;
    follow = true;
    object->attachCamera(id);
}

void Camera::stopFollowing(){
    object->detachCamera();
    object = NULL;
    follow = false;
}

int Camera::getWidth() const {
    return window->getWidth();
}

int Camera::getHeight() const {
    return window->getHeight();
}

void Camera::act(){
    // Update camera to whatever object it may be following or to set destination
    if (smoothScrolling){
        if (smoothScrollWaitX > 0){
            smoothScrollWaitX--;
        } else {
            currentX = (currentX+x)/2;
            smoothScrollWaitX = smoothScrollModifier;
        }
        if (smoothScrollWaitY > 0){
            smoothScrollWaitY--;
        } else {
            currentY = (currentY+y)/2;
            smoothScrollWaitY = smoothScrollModifier;
        }
    } else {
        if (currentX < x){
            currentXSpeed+=velocity;
            currentX+=scrollSpeed+currentXSpeed;
            if (currentX > x){
                currentX = x;
                currentXSpeed = 0;
            }
        } else if (currentX > x){
            currentXSpeed+=velocity;
            currentX-=scrollSpeed+currentXSpeed;
            if (currentX < x){
                currentX = x;
                currentXSpeed = 0;
            }
        }
        
        if (currentY < y){
            currentYSpeed+=velocity;
            currentY+=scrollSpeed+currentYSpeed;
            if (currentY > y){
                currentY = y;
                currentYSpeed = 0;
            }
        } else if (currentY > y){
            currentYSpeed+=velocity;
            currentY-=scrollSpeed+currentYSpeed;
            if (currentY < y){
                currentY = y;
                currentYSpeed = 0;
            }
        }
    }
}

void Camera::draw(const Graphics::Bitmap & work){
    window->Blit(windowX, windowY, work);
}

void Camera::checkBounds(){
    if (x < 0){
        x = 0;
    } else if (x > (worldWidth - getViewportWidth())){
        x = (worldWidth - getViewportWidth());
    }
    if (y < 0){
        y = 0;
    } else if (y > (worldHeight - getViewportHeight())){
        y = (worldHeight - getViewportHeight());
    }
}
