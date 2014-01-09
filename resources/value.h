#ifndef platformer_value_h
#define platformer_value_h

#include <string>

namespace Platformer{

class Value{
public:
    enum Type{
        String,
        Int,
        Double,
        Boolean,
    };
    Value();
    Value(const std::string &);
    Value(int value);
    Value(double value);
    Value(bool value);
    Value(const Value &);
    
    ~Value();
    
    const Value & operator=(const Value &);
    const Value & operator=(const char *);
    const Value & operator=(const std::string &);
    const Value & operator=(int value);
    const Value & operator=(double value);
    const Value & operator=(bool value);
    
    const Value & operator>>(std::string &);
    const Value & operator>>(int value);
    const Value & operator>>(double value);
    const Value & operator>>(bool value);
    
    const std::string & toString() const;
    int toInt() const;
    double toDouble() const;
    bool toBool() const;
    
    void clear();
    
    inline const Type & getType() const{
        return this->type;
    }
    
private:
    Type type;
    std::string value;
};

}

#endif
