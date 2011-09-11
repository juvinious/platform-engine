#include "animation.h"

#include <sstream>
#include "util/token.h"
#include "util/trans-bitmap.h"
#include "util/bitmap.h"
#include "util/debug.h"
#include "util/funcs.h"
#include "util/file-system.h"

#include "util/load_exception.h"

using namespace std;
using namespace Platformer;

Frame::Frame(const Token *the_token, imageMap &images):
bmp(0),
time(0),
horizontalFlip(false),
verticalFlip(false),
alpha(255){
    if ( *the_token != "frame" ){
        throw LoadException(__FILE__, __LINE__, "Not an frame");
    }
    const Token & tok = *the_token;
    /* Frame
	use image -1 to not draw anything, it can be used to get a blinking effect
	use time -1 to draw infinitly or until animation is reset
	
	(frame 
	    (image NUM) 
	    (alpha NUM) 
	    (hflip 0|1) 
	    (vflip 0|1) 
	    (time NUM))
    */
    TokenView view = tok.view();
    while (view.hasMore()){
        try{
            const Token * token;
            view >> token;
            if (*token == "image"){
                // get the number
                int num;
                token->view() >> num;
                // now assign the bitmap
                bmp = images[num];
            } else if (*token == "alpha"){
                // get alpha
                token->view() >> alpha;
            } else if (*token == "hflip"){
                // horizontal flip
                token->view() >> horizontalFlip;
            } else if (*token == "vflip"){
                // horizontal flip
                token->view() >> verticalFlip;
            } else if (*token == "time"){
                // time to display
                token->view() >> time;
            } else {
                Global::debug( 3 ) << "Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Animation parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

Frame::Frame(Graphics::Bitmap * bmp):
bmp(bmp),
time(0),
horizontalFlip(false),
verticalFlip(false),
alpha(255){
}

Frame::~Frame(){
}

static void renderSprite(int x, int y, const Graphics::Bitmap & sprite, int alpha, bool horizontalFlip, bool verticalFlip, const Graphics::Bitmap & where){
    if (alpha != 255){
        Graphics::Bitmap::transBlender( 0, 0, 0, alpha );
        if (horizontalFlip && !verticalFlip){
            sprite.translucent().drawHFlip(x,y, where);
        } else if (!horizontalFlip && verticalFlip){
            sprite.translucent().drawVFlip(x,y, where);
        } else if (horizontalFlip && verticalFlip){
            sprite.translucent().drawHVFlip(x,y, where);
        } else if (!horizontalFlip && !verticalFlip){
            sprite.translucent().draw(x,y, where);
        }
    } else {
        if (horizontalFlip && !verticalFlip){
            sprite.drawHFlip(x,y, where);
        } else if (!horizontalFlip && verticalFlip){
            sprite.drawVFlip(x,y, where);
        } else if (horizontalFlip && verticalFlip){
            sprite.drawHVFlip(x,y, where);
        } else if (!horizontalFlip && !verticalFlip){
            sprite.draw(x,y, where);
        }
    }
}

void Frame::draw(int x, int y, const Graphics::Bitmap & work){
    if (!bmp){
        return;
    }
    renderSprite(x, y, *bmp, alpha, horizontalFlip, verticalFlip, work);
}

static int findPosition(int end, int offset){
    while (true){
	offset-=end;
	if (offset < end){
	    return offset;
	}
    }
}

void Frame::drawRepeatable(int x, int y, const Graphics::Bitmap & work){
    if (!bmp){
        return;
    }
    
    const int w = work.getWidth();
    const int h = work.getHeight();
    Graphics::Bitmap temp = Graphics::Bitmap::temporaryBitmap(w,h);
    int x2 = 0;
    int y2 = 0;
    if (( (x + w) > bmp->getWidth()) || ((y + h) > bmp->getHeight()) ){
	// Original position
	if (x < bmp->getWidth()){
	    bmp->Blit(x,y,w,h,0,0,temp);
	}
	// Beyond X + W
	if (x+w > bmp->getWidth()){
	    bmp->Blit(findPosition(bmp->getWidth(),x),y,w,h,0,0,temp);
	}
	// Beyond Y + H
	if (y+h > bmp->getHeight()){
	    bmp->Blit(x,findPosition(bmp->getHeight(),y),w,h,0,0,temp);
	}	
    } else {
	bmp->Blit(x,y,w,h,0,0,temp);
    }
    
    renderSprite(0, 0, temp, alpha, horizontalFlip, verticalFlip, work);
}

Animation::Animation(const Token *the_token):
ticks(0),
currentFrame(0),
loop(0){
    images[-1] = 0;
    std::string basedir = "";
    if ( *the_token != "animation" ){
        throw LoadException(__FILE__, __LINE__, "Not an animation");
    }
    /* Animation:
	The images must be listed prior to listing any frames, basedir can be used to set the directory where the images are located
	loop will begin at the subsequent frame listed after loop
	
	(anim (id NUM) 
	      (basedir LOCATION)
	      (image NUM FILE) 
	      (frame "Read comments above in constructor") 
	      (loop NUM)
    */
    const Token & tok = *the_token;
    TokenView view = tok.view();
    while (view.hasMore()){
        try{
            const Token * token;
            view >> token;
            if (*token == "id"){
                // get the name
                token->view() >> id;
            } else if (*token == "basedir"){
                // set the base directory for loading images
                token->view() >> basedir;
            } else if (*token == "image"){
                // add bitmaps by number to the map
                int number;
                std::string temp;
                token->view() >> number >> temp;
		Global::debug(1, "Platformer") << "Loading image for animation number: " << id << ". Image: " << temp << endl;
                Graphics::Bitmap *bmp = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(basedir + temp)).path());
                if (bmp->getError()){
                    delete bmp;
                } else {
                    images[number] = bmp;
                }
            } else if (*token == "frame"){
                // new frame
                Frame *frame = new Frame(token, images);
                frames.push_back(frame);
            } else if (*token == "loop"){
                // start loop here
                int l;
                token->view() >> l;
		if (l >= (int)frames.size()){
		    ostringstream out;
		    out << "Loop location is larger than the number of frames. Loop: " << loop << " Frames: " << frames.size();
		    throw LoadException(__FILE__, __LINE__, out.str());
		}
		loop = l;
            } else {
                Global::debug( 3 ) << "Unhandled Animation attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Animation parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
    if (id.empty()){
	throw LoadException(__FILE__, __LINE__, "The animation has no identification, give it a name.");
    }
}

Animation::~Animation(){
    for (std::vector<Frame *>::iterator i = frames.begin(); i != frames.end(); ++i){
        if (*i){
            delete *i;
        }
    }

    for (imageMap::iterator i = images.begin(); i != images.end(); ++i){
        if (i->second){
            delete i->second;
        }
    }
}
void Animation::act(){
    if( frames[currentFrame]->getTime() != -1 ){
	    ticks++;
	    if(ticks >= frames[currentFrame]->getTime()){
		    ticks = 0;
		    forwardFrame();
	    }
    }
}
void Animation::draw(int x, int y, const Graphics::Bitmap & work){
    frames[currentFrame]->draw(x, y,work);
}

void Animation::drawRepeatable(int x, int y, const Graphics::Bitmap & work){
    frames[currentFrame]->drawRepeatable(x, y,work);
}

void Animation::forwardFrame(){
    if (currentFrame < frames.size() -1){
        currentFrame++;
    } else {
        currentFrame = loop;
    }
}
void Animation::backFrame(){
    if (currentFrame > loop){
	    currentFrame--;
    } else {
	    currentFrame = frames.size() - 1;
    }
}

const Graphics::Bitmap & Animation::getBitmap() const {
    return frames[currentFrame]->getBitmap();
}
