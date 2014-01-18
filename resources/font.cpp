#include "font.h"

#include "util/debug.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/graphics/bitmap.h"
#include "util/exceptions/load_exception.h"
#include "util/token.h"
#include "util/tokenreader.h"

#include <sstream>

using namespace Platformer::Font;

Character::Character(char character, Util::ReferenceCount<Graphics::Bitmap> image):
character(character),
image(image){
}

Character::~Character(){
}

void Character::render(const Graphics::Bitmap & work, int x, int y, double scale){
    image->drawStretched(x, y, getWidth(scale), getHeight(scale), work);
}

int Character::getWidth(double scale) const{
    return (int)image->getWidth() * scale;
}

int Character::getHeight(double scale) const{
    return (int)image->getHeight() * scale;
}

Renderer::Renderer(const std::string & name, const std::string & filename, const std::string & characterMap, bool ignoreCase):
name(name),
filename(filename),
ignoreCase(ignoreCase){
    // load it
    load(characterMap);
}

Renderer::Renderer(const Token * token):
ignoreCase(false){
    if ( *token != "font" ){
        throw LoadException(__FILE__, __LINE__, "Not a font!");
    }

    std::string characterMap;
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "name"){
                tok->view() >> name;
            } else if (*tok == "filename"){
                tok->view() >> filename;
            } else if (*tok == "character-map"){
                tok->view() >> characterMap;
                Global::debug(2) << "Character Map [" << characterMap << "]" << std::endl;
            } else if (*tok == "ignore-case"){
                tok->view() >> ignoreCase;
            } else {
                Global::debug(3) << "Unhandled Font attribute: " << std::endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Font parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
    
    if (name.empty()){
        throw LoadException(__FILE__, __LINE__, "Unamed font.");
    }
    
    // Load it up
    load(characterMap);
}

Renderer::~Renderer(){
}


struct Point{
    Point():x(0),y(0){}
    int x;
    int y;
};

static Util::ReferenceCount<Graphics::Bitmap> getNext(Point & location, const Graphics::Color & seperator, const Graphics::Bitmap & image){
    int startx = location.x;
    int starty = location.y;
    
    for (; image.getPixel(startx, starty) == seperator && startx < image.getWidth(); startx++);
    for (; image.getPixel(startx, starty) == seperator && starty < image.getHeight(); starty++);
    int endx = startx;
    int endy = starty;
    for (; image.getPixel(endx, endy) != seperator && endx < image.getWidth(); endx++);
    for (; image.getPixel(startx, endy) != seperator && endy < image.getHeight(); endy++);
    int width = endx - startx;
    int height = endy - starty;
    
    Util::ReferenceCount<Graphics::Bitmap> character = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(width, height));
    Graphics::Bitmap temp(image, startx, starty, width, height);
    temp.Blit(*character.raw());
    
    int endcheck = endx + 1;
    for (; image.getPixel(endcheck, starty) == seperator && endcheck < image.getWidth(); endcheck++);
    
    if (endcheck >= image.getWidth()){
        location.x = 0;
        location.y = endy + 1;
    } else {
        location.x = endx + 1;
    }
    
    return character;
    
}

void Renderer::load(std::string characterMap){
    Graphics::Bitmap temp = Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(filename)).path());
    if (temp.getError()){
        throw LoadException(__FILE__, __LINE__, "Could not load image [" + filename + "] for font.");
    }
    
    Graphics::Color seperator = temp.getPixel(0,0);
    
    if (ignoreCase){
        characterMap = Util::lowerCaseAll(characterMap);
    }
    
    // Load it up
    Point location;
    for (unsigned int i = 0; i < characterMap.size() && location.y < temp.getHeight(); i++){
        Util::ReferenceCount<Graphics::Bitmap> charImage = getNext(location, seperator, temp);
        characters[characterMap[i]] = Util::ReferenceCount<Character>(new Character(characterMap[i], charImage));
    }
}

void Renderer::render(const Graphics::Bitmap & work, int x, int y, int alignment, double scale, const std::string & text, ...){
    
    std::ostringstream str;
    
    va_list ap;
    va_start(ap, text);
    
    // Parse args
    for (unsigned int i = 0; i < text.size();i++){
        if (text[i]=='%'){
            if(text[i+1]=='s'){
                str << va_arg(ap,char *);
                ++i;
            }
            else if(text[i+1]=='d' || text[i+1]=='i'){
                str << va_arg(ap,signed int);
                ++i;
            }
            else if(text[i+1]=='c'){
                str << (char)va_arg(ap,int);
                ++i;
            } else {
                str << text[i];
            }
        } else {
            str << text[i];
        }
    }
    va_end(ap);
    
    const std::string renderText = (ignoreCase ? Util::lowerCaseAll(str.str()) : str.str());
    
    Point location;
    
    switch(alignment){
        case 0:
            location.x = x - getLength(renderText, scale)/2;
            location.y = y;
            break;
        case -1:
            location.x = x - getLength(renderText, scale);
            location.y = y;
            break;
        case 1:
        default:
            location.x = x;
            location.y = y;
            break;
    }
    
    CharacterMap::iterator nextCharacter;
    for(unsigned int i = 0; i < renderText.length();i++){
        nextCharacter = characters.find(renderText[i]);
        if (nextCharacter != characters.end()){
            nextCharacter->second->render(work, location.x, location.y, scale);
            location.x += nextCharacter->second->getWidth(scale);
        }
    }
}

int Renderer::getLength(const std::string & text, double scale){
    int total = 0;
    for (unsigned int i = 0; i < text.size(); i++){
        CharacterMap::iterator check = characters.find(text[i]);
        if (check != characters.end()){
            total += check->second->getWidth(scale);
        }
    }
    
    return total;
}

Map::FontMap Map::fonts;

void Map::add(const std::string & name, const std::string & filename, const std::string & characterMap, bool ignoreCase){
    Util::ReferenceCount<Renderer> font = Util::ReferenceCount<Renderer>(new Renderer(name, filename, characterMap, ignoreCase));
    fonts.insert(std::pair<std::string, Util::ReferenceCount<Renderer> >(font->getName(), font));
}

void Map::add(const Token * token){
    Util::ReferenceCount<Renderer> font = Util::ReferenceCount<Renderer>(new Renderer(token));
    fonts.insert(std::pair<std::string, Util::ReferenceCount<Renderer> >(font->getName(), font));
}

// Returns first instance
Util::ReferenceCount<Renderer> Map::get(){
    FontMap::iterator check = fonts.begin();
    if (check != fonts.end()){
        return check->second;
    }
    
    return Util::ReferenceCount<Renderer>(NULL);
}

Util::ReferenceCount<Renderer> Map::get(const std::string & name){
    FontMap::iterator check = fonts.find(name);
    if (check != fonts.end()){
        return check->second;
    }
    
    return Util::ReferenceCount<Renderer>(NULL);
}

void Map::clear(){
    fonts.clear();
}
