#ifndef platformer_resource_control_h
#define platformer_resource_control_h

namespace Platformer{

class Control{
public:
    Control(){}
    virtual ~Control(){}
    
    virtual void act() = 0;
    
    virtual int getID() const = 0;
    
    virtual bool up() = 0;
    virtual bool down() = 0;
    virtual bool left() = 0;
    virtual bool right() = 0;
    virtual bool start() = 0;
    virtual bool select() = 0;
    virtual bool escape() = 0;
    virtual bool button1() = 0;
    virtual bool button2() = 0;
    virtual bool button3() = 0;
    virtual bool button4() = 0;
};

}
#endif
