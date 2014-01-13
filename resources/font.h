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
namespace Font{

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

class Renderer{
public:
    Renderer(const std::string &, const std::string &, const std::string &, bool ignoreCase);
    Renderer(const Token *);
    ~Renderer();
    
    void load(std::string characterMap);
    
    void render(const Graphics::Bitmap &, int x, int y, int alignment, double scale, const std::string &, ...);
    int getLength(const std::string &, double scale);
    
    inline const std::string & getName() const {
        return this->name;
    }

private:
    std::string name;
    std::string filename;
    bool ignoreCase;
    typedef std::map<char, Util::ReferenceCount<Character> > CharacterMap;
    CharacterMap characters;
};

class Map{
public:
    static void add(const std::string &, const std::string &, const std::string &, bool ignoreCase);
    static void add(const Token *);
    static Util::ReferenceCount<Renderer> get();
    static Util::ReferenceCount<Renderer> get(const std::string &);
    
private:
    Map();
    ~Map();
    typedef std::map<std::string, Util::ReferenceCount<Renderer> > FontMap;
    static FontMap fonts;
};

}
}
#endif
