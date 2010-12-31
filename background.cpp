#include "platformer/background.h"
#include "util/bitmap.h"
#include "util/token.h"
#include "platformer/camera.h"
#include "platformer/tile.h"

using namespace Platformer;

Background::Background(Token * token):
type(Tiles),
image(0){
}

Background::~Background(){
}

void Background::act(){
}

void Background::draw(const Camera & camera, const Bitmap & bmp){
}