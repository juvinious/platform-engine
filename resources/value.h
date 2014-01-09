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
    
    const Value & operator=(const Value &) const;
    const Value & operator=(const char *) const;
    const Value & operator=(const std::string &) const;
    const Value & operator=(int value) const;
    const Value & operator=(double value) const;
    const Value & operator=(bool value) const;
    
    const Value & operator>>(std::string &) const;
    const Value & operator>>(int value) const;
    const Value & operator>>(double value) const;
    const Value & operator>>(bool value) const;
    
    const std::string & toString() const;
    int toInt() const;
    double toDouble() const;
    bool toBool() const;
    
    void clear();
    
    inline const Type & getType() const{
        return this->type;
    }
    
private:
    mutable Type type;
    mutable std::string value;
};

}

#endif
