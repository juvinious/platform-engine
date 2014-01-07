#include "object.h"

#include <Python.h>

using namespace Platformer;

Runnable::Runnable(const std::string module, const std::string function):
module(module),
function(function){
}

Runnable::Runnable(const Runnable & copy):
module(copy.module),
function(copy.function){
}

const Runnable & Runnable::operator=(const Runnable & copy){
    module = copy.module;
    function = copy.function;
    
    return *this;
}

static PyMethodDef ObjectMethods[] = {
    //{"init",  initObject, METH_VARARGS, "Initialize object."},
    {NULL, NULL, 0, NULL}
};

ScriptObject::ScriptObject(){
}

ScriptObject::~ScriptObject(){
}

void ScriptObject::act(const Util::ReferenceCount<Platformer::CollisionMap> collisionMap){
    // Act
    
}

void ScriptObject::draw(const Platformer::Camera & camera){
}
