#include "camera.h"

#include "util/bitmap.h"
#include "util/token.h"

using namespace Platformer;

Camera::Camera(int worldWidth, int worldHeight):
worldWidth(worldWidth),
worldHeight(worldHeight),
x(0),
y(0),
windowX(0),
windowY(0),
windowWidth(0),
windowHeight(0),
scrollSpeed(2),
follow(false),
followVariance(1.5){
}

Camera::~Camera(){
}

void Camera::set(int x, int y){
}

void Camera::move(int x, int y){
}

void Camera::act(){
}