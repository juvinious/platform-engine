#ifndef platformer_font_h
#define platformer_font_h

#include <string>
#include <map>
#include <cstdarg>

#include "util/pointer.h"

class Token;

namespace Graphics{
class Bitmap;
}

namespace Platformer{
namespace Text{

class Character{
public:
    Character(char character, Util::ReferenceCount<Graphics::Bitmap>);
    ~Character();
    
    void render(const Graphics::Bitmap &, int x, int y, double scale);
    
    int getWidth(double scale) const;
    int getHeight(double scale) const;
    
private:
    char character;
    Util::ReferenceCount<Graphics::Bitmap> image;
};

class Font{
public:
    Font(const Token *);
    ~Font();
    
    void render(const Graphics::Bitmap &, int x, int y, int alignment, double scale, const std::string &, ...);
    int getLength(const std::string &, double scale);

private:
    std::string filename;
    typedef std::map<char, Util::ReferenceCount<Character> > CharacterMap;
    CharacterMap characters;
};
    
}
}
#endif
