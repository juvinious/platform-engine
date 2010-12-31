#include "platformer/animation.h"

#include <sstream>
#include "util/token.h"
#include "util/bitmap.h"
#include "util/debug.h"
#include "util/funcs.h"
#include "util/file-system.h"

using namespace std;
using namespace Platformer;

Frame::Frame(const Token *the_token, imageMap &images) throw (LoadException):
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

Frame::Frame(Bitmap * bmp):
bmp(bmp),
time(0),
horizontalFlip(false),
verticalFlip(false),
alpha(255){
}

Frame::~Frame(){
}

void Frame::draw(int x, int y, const Bitmap & work){
    if (!bmp){
        return;
    }
    if (alpha != 255){
        Bitmap::transBlender( 0, 0, 0, alpha );
        if (horizontalFlip && !verticalFlip){
            bmp->drawTransHFlip(x,y, work);
        } else if (!horizontalFlip && verticalFlip){
            bmp->drawTransVFlip(x,y, work);
        } else if (horizontalFlip && verticalFlip){
            bmp->drawTransHVFlip(x,y, work);
        } else if (!horizontalFlip && !verticalFlip){
            bmp->drawTrans(x,y, work);
        }
    } else {
        if (horizontalFlip && !verticalFlip){
            bmp->drawHFlip(x,y, work);
        } else if (!horizontalFlip && verticalFlip){
            bmp->drawVFlip(x,y, work);
        } else if (horizontalFlip && verticalFlip){
            bmp->drawHVFlip(x,y, work);
        } else if (!horizontalFlip && !verticalFlip){
            bmp->draw(x,y, work);
        }
    }
}

Animation::Animation(const Token *the_token) throw (LoadException):
ticks(0),
currentFrame(0),
loop(0){
    images[-1] = 0;
    std::string basedir = "";
    if ( *the_token != "anim" ){
        throw LoadException(__FILE__, __LINE__, "Not an animation");
    }
    /* Animation:
	The images must be listed prior to listing any frames, basedir can be used to set the directory where the images are located
	loop will begin at the subsequent frame listed after loop
	
	(anim (name Name) 
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
            if (*token == "name"){
                // get the name
                token->view() >> name;
            } else if (*token == "basedir"){
                // set the base directory for loading images
                token->view() >> basedir;
            } else if (*token == "image"){
                // add bitmaps by number to the map
                int number;
                std::string temp;
                token->view() >> number >> temp;
                Bitmap *bmp = new Bitmap(Filesystem::find(Filesystem::RelativePath(basedir + temp)).path());
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
                loop = frames.size();
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
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "The animation has no name.");
    }
    if (loop >= frames.size()){
        ostringstream out;
        out << "Loop location is larger than the number of frames. Loop: " << loop << " Frames: " << frames.size();
        throw LoadException(__FILE__, __LINE__, out.str());
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
void Animation::draw(int x, int y, const Bitmap & work){
    frames[currentFrame]->draw(x, y,work);
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
