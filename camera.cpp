#include "camera.h"

#include "util/bitmap.h"
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
window(0),
scrollSpeed(2),
currentXSpeed(0),
currentYSpeed(0),
velocity(.5),
follow(false),
followVariance(1.5){
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
    window = new Graphics::Bitmap(resolutionX, resolutionY);
}

Camera::~Camera(){
    if (window){
	delete window;
    }
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

void Camera::act(){
    // Update camera to whatever object it may be following or to set destination
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

void Camera::draw(const Graphics::Bitmap & work){
    window->Blit(work);
    if (windowY > 0){
        work.rectangleFill(0, 0, windowWidth, windowY, Graphics::makeColor(0,0,0));
    }
    if (windowX > 0){
        work.rectangleFill(0, windowY, windowX, windowHeight, Graphics::makeColor(0,0,0));
    }
    if (windowWidth < resolutionX){
        work.rectangleFill(windowWidth, 0, resolutionX, resolutionY, Graphics::makeColor(0,0,0));
    }
    if (windowHeight < resolutionY){
        work.rectangleFill(0, windowHeight, windowWidth, resolutionY, Graphics::makeColor(0,0,0));
    }
}

void Camera::checkBounds(){
    if (x < 0){
	x = 0;
    } else if (x > (worldWidth - windowWidth)){
	x = (worldWidth - windowWidth);
    }
    if (y < 0){
	y = 0;
    } else if (y > (worldHeight - windowHeight)){
	y = (worldHeight - windowHeight);
    }
}
