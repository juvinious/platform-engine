#ifndef platformer_script_control_h
#define platformer_script_control_h

#ifdef HAVE_PYTHON
#include <Python.h>

#include "platformer/resources/control.h"
#include "util/input/input-map.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"

namespace Platformer{
namespace Script{

class Control : public Platformer::Control{
public:
    Control(int id);
    ~Control();
    
    void act();
    
    int getID() const;
    
    bool up();
    bool down();
    bool left();
    bool right();
    bool start();
    bool select();
    bool escape();
    bool button1();
    bool button2();
    bool button3();
    bool button4();
    
    static PyMethodDef * Methods;
private:
    int id;
    bool keyUp;
    bool keyDown;
    bool keyLeft;
    bool keyRight;
    bool keyStart;
    bool keySelect;
    bool keyEscape;
    bool keyButton1;
    bool keyButton2;
    bool keyButton3;
    bool keyButton4;
    
    //! keys
    enum Keys{
        Up,
        Down,
        Left,
        Right,
        Esc,
        Select,
        Start,
        Button1,
        Button2,
        Button3,
        Button4,
    };
    InputMap<Keys> input;
    InputSource source;
};

}
}
#endif
#endif
