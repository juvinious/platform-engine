#ifdef HAVE_PYTHON
#include "control.h"

using namespace Platformer::Script;

static PyObject * getID(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->getID());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * up(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->up());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * down(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->down());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * left(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->left());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * right(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->right());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * escape(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->escape());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * start(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->start());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * select(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->select());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * button1(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->button1());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * button2(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->button2());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * button3(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->button3());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * button4(PyObject *, PyObject * args){
    PyObject * controlPointer;
    if (PyArg_ParseTuple(args, "O", &controlPointer)){
        Platformer::Script::Control * ctrl = reinterpret_cast<Platformer::Script::Control*>(PyCapsule_GetPointer(controlPointer, "control"));
        return Py_BuildValue("i", ctrl->button4());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef ControlMethods[] = {
    {"getID", getID, METH_VARARGS, "Get ID."},
    {"up", up, METH_VARARGS, "Get up state."},
    {"down", down, METH_VARARGS, "Get down state."},
    {"left", left, METH_VARARGS, "Get left state."},
    {"right", right, METH_VARARGS, "Get right state."},
    {"escape", escape, METH_VARARGS, "Get escape state."},
    {"select", select, METH_VARARGS, "Get select state."},
    {"start", start, METH_VARARGS, "Get start state."},
    {"button1", button1, METH_VARARGS, "Get button1 state."},
    {"button2", button2, METH_VARARGS, "Get button2 state."},
    {"button3", button3, METH_VARARGS, "Get button3 state."},
    {"button4", button4, METH_VARARGS, "Get button4 state."},
    {NULL,NULL,0,NULL},
};

PyMethodDef * Control::Methods = ControlMethods;

Control::Control(int id):
id(id),
keyUp(false),
keyDown(false),
keyLeft(false),
keyRight(false),
keyStart(false),
keySelect(false),
keyEscape(false),
keyButton1(false),
keyButton2(false),
keyButton3(false),
keyButton4(false),
source(true){
    // FIXME figure out a way to map these so that they are configurable in a script
    input.set(Keyboard::Key_UP, Up);
    input.set(Keyboard::Key_DOWN, Down);
    input.set(Keyboard::Key_LEFT, Left);
    input.set(Keyboard::Key_RIGHT, Right);
    input.set(Keyboard::Key_ESC, Esc);
    input.set(Keyboard::Key_TAB, Select);
    input.set(Keyboard::Key_ENTER, Start);
    input.set(Keyboard::Key_LCONTROL, Button1);
    input.set(Keyboard::Key_ALT, Button2);
    input.set(Keyboard::Key_1, Button3);
    input.set(Keyboard::Key_2, Button4);
}

Control::~Control(){
}

void Control::act(){
    // Keys
    class Handler: public InputHandler<Keys> {
    public:
        Handler(Control & self):
        self(self){
        }

        Control & self;

        void release(const Keys & input, Keyboard::unicode_t unicode){
            switch (input){
                case Up: {
                    self.keyUp = false;
                    break;
                }
                case Down: {
                    self.keyDown = false;
                    break;
                }
                case Left: {
                    self.keyLeft = false;
                    break;
                }
                case Right: {
                    self.keyRight = false;
                    break;
                }
                case Esc:
                    self.keyEscape = false;
                    break;
                case Select: {
                    self.keySelect = false;
                    break;
                }
                case Start: {
                    self.keyStart = false;
                    break;
                }
                case Button1: {
                    self.keyButton1 = false;
                    break;
                }
                case Button2: {
                    self.keyButton2 = false;
                    break;
                }
                case Button3: {
                    self.keyButton3 = false;
                    break;
                }
                case Button4: {
                    self.keyButton4 = false;
                    break;
                }
            }
        }

        void press(const Keys & input, Keyboard::unicode_t unicode){
            switch (input){
                case Up: {
                    self.keyUp = true;
                    break;
                }
                case Down: {
                    self.keyDown = true;
                    break;
                }
                case Left: {
                    self.keyLeft = true;
                    break;
                }
                case Right: {
                    self.keyRight = true;
                    break;
                }
                case Esc:
                    self.keyEscape = true;
                    break;
                case Select: {
                    self.keySelect = true;
                    break;
                }
                case Start: {
                    self.keyStart = true;
                    break;
                }
                case Button1: {
                    self.keyButton1 = true;
                    break;
                }
                case Button2: {
                    self.keyButton2 = true;
                    break;
                }
                case Button3: {
                    self.keyButton3 = true;
                    break;
                }
                case Button4: {
                    self.keyButton4 = true;
                    break;
                }
            }
        }
    };

    Handler handler(*this);
    InputManager::handleEvents(input, source, handler);
}

int Control::getID() const{
    return id;
}

bool Control::up(){
    return keyUp;
}

bool Control::down(){
    return keyDown;
}

bool Control::left(){
    return keyLeft;
}

bool Control::right(){
    return keyRight;
}

bool Control::start(){
    return keyStart;
}

bool Control::select(){
    return keySelect;
}

bool Control::escape(){
    return keyEscape;
}

bool Control::button1(){
    return keyButton1;
}

bool Control::button2(){
    return keyButton2;
}

bool Control::button3(){
    return keyButton3;
}

bool Control::button4(){
    return keyButton4;
}


#endif
