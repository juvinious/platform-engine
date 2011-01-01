#include "background.h"
#include "util/bitmap.h"
#include "util/token.h"
#include "camera.h"
#include "tile.h"

using namespace Platformer;

Background::Background(const Token * token):
type(Tiles),
image(0){
}

Background::~Background(){
}

void Background::act(){
}

void Background::draw(const Camera & camera, const Bitmap & bmp){
}